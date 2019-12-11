// Fill out your copyright notice in the Description page of Project Settings.

#include "XD_EnvironmentManager.h"
#include <Net/UnrealNetwork.h>
#include <GameFramework/GameStateBase.h>
#include <EngineUtils.h>
#include <FXSystem.h>
#include <Engine/ActorChannel.h>
#include <Components/VectorFieldComponent.h>
#include <VectorField/VectorFieldStatic.h>
#include <VectorField/VectorFieldVolume.h>
#include <Components/WindDirectionalSourceComponent.h>

#include "XD_EnvironmentGameStateInterface.h"
#include "XD_ActorFunctionLibrary.h"
#include "XD_EnvironmentSystemUtility.h"
#include "XD_EnvironmentSystemSettings.h"
#include "XD_EnvironmentPreviewActor.h"
#include "SubManager/XD_InsideManagerActor.h"


// Sets default values for this component's properties
UXD_EnvironmentManager::UXD_EnvironmentManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UXD_EnvironmentManager::BeginPlay()
{
	Super::BeginPlay();

	// ...
	for (UXD_EnvironmentSubManager* SubManager : SubManagers)
	{
		if (SubManager)
		{
			SubManager->ReceiveBeginPlay();
		}
	}
}

void UXD_EnvironmentManager::OnRegister()
{
	Super::OnRegister();

#if WITH_EDITOR
	struct FEditorSpawnPreviewActorControlGuard
	{
		UWorld* World;
		FDelegateHandle SpawnActorHandle;
		FEditorSpawnPreviewActorControlGuard(UWorld* World, AActor* AuthoriyActor)
			:World(World)
		{
			if (World && World->WorldType == EWorldType::Editor)
			{
				SpawnActorHandle = World->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateLambda([AuthoriyActor, this](AActor* Actor)
				{
					Actor->bIsEditorOnlyActor = true;
					Actor->SetOwner(AuthoriyActor);
					Actor->AttachToActor(AuthoriyActor, FAttachmentTransformRules::KeepWorldTransform);
				}));
			}
		}

		~FEditorSpawnPreviewActorControlGuard()
		{
			World->RemoveOnActorSpawnedHandler(SpawnActorHandle);
		}
	};

	FEditorSpawnPreviewActorControlGuard EditorSpawnPreviewActorControlGuard(GetWorld(), GetOwner());
	if (GetWorld() && GetWorld()->WorldType == EWorldType::Editor)
	{
		PreviewEnvironmentManager = this;
	}
#endif

	FGuardValue_Bitfield(GetWorld()->bIsRunningConstructionScript, false);
	for (UXD_EnvironmentSubManager* SubManager : SubManagers)
	{
		if (SubManager)
		{
			SubManager->ReceiveOnConstruction(GetOwner()->GetActorTransform());
		}
	}

	if (InsideManager == nullptr)
	{
		if (TSubclassOf<class AXD_InsideManagerActor> InsideManagerActorClass = GetDefault<UXD_EnvironmentSystemSettings>()->InsideManagerActorClass.LoadSynchronous())
		{
			InsideManager = GetWorld()->SpawnActor<AXD_InsideManagerActor>(InsideManagerActorClass);
		}
		else
		{
			EnvironmentSystem_Warning_LOG("未配置室内区分管理器(AXD_InsideManagerActor)，请去配置项中设置");
		}
	}

	if (SnowPathManager == nullptr)
	{
		if (TSubclassOf<class AXD_SnowPathManagerActorBase> SnowPathManagerActorClass = GetDefault<UXD_EnvironmentSystemSettings>()->SnowPathManagerActorClass.LoadSynchronous())
		{
			SnowPathManager = GetWorld()->SpawnActor<AXD_SnowPathManagerActorBase>(SnowPathManagerActorClass);
		}
		else
		{
			EnvironmentSystem_Warning_LOG("未配置积雪路径管理器(AXD_SnowPathManagerActorBase)，请去配置项中设置");
		}
	}

	SubManagerActors.Remove(nullptr);
	for (const TSoftClassPtr<AActor>& ActorClass : GetDefault<UXD_EnvironmentSystemSettings>()->SubManagerActorClasses)
	{
		TSubclassOf<AActor> ActorClassInstance = ActorClass.LoadSynchronous();
		if (ActorClassInstance && !SubManagerActors.ContainsByPredicate([&](AActor* Actor) {return Actor->IsA(ActorClassInstance); }))
		{
			SubManagerActors.Add(GetWorld()->SpawnActor<AActor>(ActorClassInstance));
		}
	}

	// TODO：创建自己的VectorFieldComponent用于向管理器注册、反注册和区分类型（风、洋流）
	{
		auto RegistVectorFieldVolume = [&](AVectorFieldVolume* VectorFieldVolume)
		{
			UVectorFieldComponent* VectorFieldComponent = VectorFieldVolume->GetVectorFieldComponent();
			if (UVectorFieldStatic* StaticVectorField = Cast<UVectorFieldStatic>(VectorFieldComponent->VectorField))
			{
				const uint32 SizeX = (uint32)StaticVectorField->SizeX;
				const uint32 SizeY = (uint32)StaticVectorField->SizeY;
				const uint32 SizeZ = (uint32)StaticVectorField->SizeZ;
				const FVector4 Size(SizeX, SizeY, SizeZ, 1.0f);

				const FVector4 MinBounds(StaticVectorField->Bounds.Min.X, StaticVectorField->Bounds.Min.Y, StaticVectorField->Bounds.Min.Z, 0.f);
				const FVector BoundSize = StaticVectorField->Bounds.GetSize();
				const FVector4 OneOverBoundSize(FVector::OneVector / BoundSize, 1.0f);

				if (StaticVectorField->bAllowCPUAccess)
				{
					check(StaticVectorField->CPUData.GetData() != nullptr && FMath::Min3(SizeX, SizeY, SizeZ) > 0 && BoundSize.GetMin() > SMALL_NUMBER);
					WindVectorFields.Add(VectorFieldComponent);
					VectorFieldComponent->SetIntensity(GetWindFieldIntensity());
					VectorFieldVolume->OnDestroyed.AddUniqueDynamic(this, &UXD_EnvironmentManager::WhenVectorFieldDestroyed);
				}
			}
		};
		for (TActorIterator<AVectorFieldVolume> It(GetWorld(), AVectorFieldVolume::StaticClass()); It; ++It)
		{
			AVectorFieldVolume* VectorFieldVolume = *It;
			if (!VectorFieldVolume->IsPendingKill())
			{
				RegistVectorFieldVolume(VectorFieldVolume);
			}
		}
		OnActorSpawnedHandle = GetWorld()->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateLambda([=](AActor* Actor)
			{
				if (AVectorFieldVolume* VectorFieldVolume = Cast<AVectorFieldVolume>(Actor))
				{
					RegistVectorFieldVolume(VectorFieldVolume);
				}
			}));
	}

	// 临时创建风组件用于影响布料效果，最优方案是修改FScene::GetWindParameters_GameThread
	{
		AActor* Owner = GetOwner();
		WindDirectionalSourceComponent = NewObject<UWindDirectionalSourceComponent>(Owner, GET_MEMBER_NAME_CHECKED(UXD_EnvironmentManager, WindDirectionalSourceComponent));
		Owner->AddOwnedComponent(WindDirectionalSourceComponent);
		WindDirectionalSourceComponent->RegisterComponent();
	}
}

void UXD_EnvironmentManager::OnUnregister()
{
	Super::OnUnregister();

	GetWorld()->RemoveOnActorSpawnedHandler(OnActorSpawnedHandle);
}

// Called every frame
void UXD_EnvironmentManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (GetOwner()->GetGameTimeSinceCreation() > DeltaTime)
	{
		for (UXD_EnvironmentSubManager* SubManager : SubManagers)
		{
			if (SubManager)
			{
				SubManager->ReceiveTick(DeltaTime);
			}
		}
	}

	// 风相关的计算
	{
		if (GetOwner()->HasAuthority())
		{
			GustUpdateRemainTime -= DeltaTime;
			if (GustUpdateRemainTime <= 0.f)
			{
				GustUpdateRemainTime = FMath::RandRange(15.f, 30.f);
				TargetGustSpeed = FMath::RandRange(MinGustSpeed, MaxGustSpeed);
				GustDurationRemainTime = GustDurationTime;
			}
			if (GustDurationRemainTime > 0.f)
			{
				GustDurationRemainTime -= DeltaTime;
				if (GustDurationRemainTime <= 0.f)
				{
					TargetGustSpeed = 0.f;
				}
			}
		}
		if (GustSpeed != TargetGustSpeed)
		{
			GustSpeed = FMath::FInterpTo(GustSpeed, TargetGustSpeed, DeltaTime, GustSpeed < TargetGustSpeed ? 10.f : 1.f);
		}

		WindDirectionalSourceComponent->SetSpeed(GetGlobalWindSpeed() / 500.f);
		WindDirectionalSourceComponent->SetWorldRotation(GlobalWindVelocity.Rotation());
		for (UVectorFieldComponent* WindVectorField : WindVectorFields)
		{
			if (WindVectorField)
			{
				WindVectorField->SetIntensity(GetWindFieldIntensity());
			}
		}
	}
}

void UXD_EnvironmentManager::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UXD_EnvironmentManager, Humidity);
	DOREPLIFETIME(UXD_EnvironmentManager, Temperature);
	DOREPLIFETIME(UXD_EnvironmentManager, GlobalWindVelocity);
	DOREPLIFETIME_CONDITION(UXD_EnvironmentManager, GustSpeed, COND_InitialOnly);
	DOREPLIFETIME(UXD_EnvironmentManager, TargetGustSpeed);
	DOREPLIFETIME(UXD_EnvironmentManager, CloudsDensity);
}

bool UXD_EnvironmentManager::ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags)
{
	bool Res = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	return Res;
}

#if WITH_EDITOR
void UXD_EnvironmentManager::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
}
#endif

#if WITH_EDITOR
TWeakObjectPtr<UXD_EnvironmentManager> UXD_EnvironmentManager::PreviewEnvironmentManager;
#endif

UXD_EnvironmentManager* UXD_EnvironmentManager::GetManager(const UObject* WorldContextObject)
{
	UWorld* World = WorldContextObject->GetWorld();
#if WITH_EDITOR
	if (World == nullptr)
	{
		return nullptr;
	}
	if (World->WorldType == EWorldType::Editor)
	{
		if (PreviewEnvironmentManager.IsValid())
		{
			return PreviewEnvironmentManager.Get();
		}
		else if (TActorIterator<AXD_EnvironmentPreviewActor> It = TActorIterator<AXD_EnvironmentPreviewActor>(World, GetDefault<UXD_EnvironmentSystemSettings>()->EnvironmentPreviewActorClass.LoadSynchronous()))
		{
			AXD_EnvironmentPreviewActor* EnvironmentPreviewActor = *It;
			PreviewEnvironmentManager = EnvironmentPreviewActor->PreviewEnvironmentManager;
			return EnvironmentPreviewActor->PreviewEnvironmentManager;
		}
		else if (TSubclassOf<AXD_EnvironmentPreviewActor> EnvironmentPreviewActorClass = GetDefault<UXD_EnvironmentSystemSettings>()->EnvironmentPreviewActorClass.LoadSynchronous())
		{
			return World->SpawnActor<AXD_EnvironmentPreviewActor>(EnvironmentPreviewActorClass)->PreviewEnvironmentManager;
		}
		return nullptr;
	}
#endif
	if (AGameStateBase* GameState = World->GetGameState())
	{
		if (GameState->Implements<UXD_Environment_GameStateInterface>())
		{
			return IXD_Environment_GameStateInterface::Execute_GetEnvironmentManager(GameState);
		}
		else if (UXD_EnvironmentManager* EnvironmentManager = GameState->FindComponentByClass<UXD_EnvironmentManager>())
		{
			EnvironmentSystem_Warning_LOG("请在GameState中添加UXD_EnvironmentManager组件并继承XD_Environment_GameStateInterface接口实现GetEnvironmentManager方法");
			return EnvironmentManager;
		}
		else
		{
			return UXD_ActorFunctionLibrary::AddComponent<UXD_EnvironmentManager>(GameState);
		}
	}
	return nullptr;
}

FVector UXD_EnvironmentManager::GetWindVelocity(const FVector& Position) const 
{
	bool ContainWindField = false;

	// 风场的强度已经计算了GustScale，不需要再算了
	FVector WindFieldVelocity = FVector::ZeroVector;
	for (UVectorFieldComponent* WindVectorField : WindVectorFields)
	{
		if (WindVectorField)
		{
			TOptional<FVector> Velocity = SampleVectorField(WindVectorField, Position);
			if (Velocity)
			{
				ContainWindField = true;
				WindFieldVelocity += Velocity.GetValue();
			}
		}
	}
	return ContainWindField ? WindFieldVelocity : GetGlobalWindVelocity();
}

DECLARE_CYCLE_STAT(TEXT("SampleVectorField"), STAT_SampleVectorField, STATGROUP_ENVIRONMENTSYSTEM);

TOptional<FVector> UXD_EnvironmentManager::SampleVectorField(UVectorFieldComponent* VectorFieldComponent, const FVector& Position)
{
	SCOPE_CYCLE_COUNTER(STAT_SampleVectorField);

	// TODO：边界外的过渡
	UVectorFieldStatic* StaticVectorField = CastChecked<UVectorFieldStatic>(VectorFieldComponent->VectorField);
	const FVector Location = VectorFieldComponent->GetComponentTransform().InverseTransformPosition(Position);

	// UNiagaraDataInterfaceVectorField::SampleVectorField
	if (StaticVectorField->Bounds.IsInside(Location))
	{
		const FVector4* Data = StaticVectorField->CPUData.GetData();

		const uint32 SizeX = (uint32)StaticVectorField->SizeX;
		const uint32 SizeY = (uint32)StaticVectorField->SizeY;
		const uint32 SizeZ = (uint32)StaticVectorField->SizeZ;
		const FVector4 Size(SizeX, SizeY, SizeZ, 1.0f);

		const FVector4 MinBounds(StaticVectorField->Bounds.Min.X, StaticVectorField->Bounds.Min.Y, StaticVectorField->Bounds.Min.Z, 0.f);
		const FVector BoundSize = StaticVectorField->Bounds.GetSize();
		const FVector4 OneOverBoundSize(FVector::OneVector / BoundSize, 1.0f);
		
		// Math helper
		static auto FVector4Clamp = [](const FVector4& v, const FVector4& a, const FVector4& b) {
			return FVector4(FMath::Clamp(v.X, a.X, b.X),
				FMath::Clamp(v.Y, a.Y, b.Y),
				FMath::Clamp(v.Z, a.Z, b.Z),
				FMath::Clamp(v.W, a.W, b.W));
		};

		static auto FVector4Floor = [](const FVector4& v) {
			return FVector4(FGenericPlatformMath::FloorToFloat(v.X),
				FGenericPlatformMath::FloorToFloat(v.Y),
				FGenericPlatformMath::FloorToFloat(v.Z),
				FGenericPlatformMath::FloorToFloat(v.W));
		};

		// Position in Volume Space
		FVector4 Pos(Location.X, Location.Y, Location.Z, 0.0f);
		// Normalize position
		Pos = (Pos - MinBounds) * OneOverBoundSize;
		// Scaled position
		Pos = Pos * Size;
		// Offset by half a cell size due to sample being in the center of its cell
		Pos = Pos - FVector4(0.5f, 0.5f, 0.5f, 0.0f);

		// 
		FVector4 Index0 = FVector4Floor(Pos);
		FVector4 Index1 = Index0 + FVector4(1.0f, 1.0f, 1.0f, 0.0f);

		Index0 = FVector4Clamp(Index0, FVector4(0.0f), Size - FVector4(1.0f, 1.0f, 1.0f, 0.0f));
		Index1 = FVector4Clamp(Index1, FVector4(0.0f), Size - FVector4(1.0f, 1.0f, 1.0f, 0.0f));

		// Sample by regular trilinear interpolation:

		// TODO(mv): Optimize indexing for cache? Periodicity is problematic...
		// TODO(mv): Vectorize?
		// Fetch corners
		FVector4 V000 = Data[int(Index0.X + SizeX * Index0.Y + SizeX * SizeY * Index0.Z)];
		FVector4 V100 = Data[int(Index1.X + SizeX * Index0.Y + SizeX * SizeY * Index0.Z)];
		FVector4 V010 = Data[int(Index0.X + SizeX * Index1.Y + SizeX * SizeY * Index0.Z)];
		FVector4 V110 = Data[int(Index1.X + SizeX * Index1.Y + SizeX * SizeY * Index0.Z)];
		FVector4 V001 = Data[int(Index0.X + SizeX * Index0.Y + SizeX * SizeY * Index1.Z)];
		FVector4 V101 = Data[int(Index1.X + SizeX * Index0.Y + SizeX * SizeY * Index1.Z)];
		FVector4 V011 = Data[int(Index0.X + SizeX * Index1.Y + SizeX * SizeY * Index1.Z)];
		FVector4 V111 = Data[int(Index1.X + SizeX * Index1.Y + SizeX * SizeY * Index1.Z)];

		// 
		FVector4 Fraction = Pos - Index0;

		// Blend x-axis
		FVector4 V00 = FMath::Lerp(V000, V100, Fraction.X);
		FVector4 V01 = FMath::Lerp(V001, V101, Fraction.X);
		FVector4 V10 = FMath::Lerp(V010, V110, Fraction.X);
		FVector4 V11 = FMath::Lerp(V011, V111, Fraction.X);

		// Blend y-axis
		FVector4 V0 = FMath::Lerp(V00, V10, Fraction.Y);
		FVector4 V1 = FMath::Lerp(V01, V11, Fraction.Y);

		// Blend z-axis
		FVector4 V = FMath::Lerp(V0, V1, Fraction.Z);

		// Write final output...
		FVector Result = FVector(V.X, V.Y, V.Z);

		return Result * VectorFieldComponent->Intensity;
	}

	return TOptional<FVector>();
}

void UXD_EnvironmentManager::WhenVectorFieldDestroyed(AActor* VectorField)
{
	AVectorFieldVolume* VectorFieldVolume = CastChecked<AVectorFieldVolume>(VectorField);
	WindVectorFields.Remove(VectorFieldVolume->GetVectorFieldComponent());
}

void UXD_EnvironmentManager::SetGlobalWindSpeed(float InWindSpeed)
{
	if (ensure(InWindSpeed != 0.f) && InWindSpeed != GlobalWindVelocity.Size())
	{
		GlobalWindVelocity = GlobalWindVelocity.GetSafeNormal() * InWindSpeed;
		for (UVectorFieldComponent* WindVectorField : WindVectorFields)
		{
			if (WindVectorField)
			{
				WindVectorField->SetIntensity(GetWindFieldIntensity());
			}
		}
	}
}

class UWorld* UXD_EnvironmentSubManager::GetWorld() const
{
#if WITH_EDITOR
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return nullptr;
	}
#endif
	return GetOwner()->GetWorld();
}

class AActor* UXD_EnvironmentSubManager::GetOwner() const
{
	return GetTypedOuter<AActor>();
}
