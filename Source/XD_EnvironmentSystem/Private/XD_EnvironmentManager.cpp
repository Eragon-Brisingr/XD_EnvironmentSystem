// Fill out your copyright notice in the Description page of Project Settings.

#include "XD_EnvironmentManager.h"
#include <UnrealNetwork.h>
#include <GameFramework/GameStateBase.h>
#include "XD_EnvironmentGameStateInterface.h"
#include "XD_ActorFunctionLibrary.h"
#include "XD_EnvironmentSystemUtility.h"
#include <Engine/ActorChannel.h>
#include "XD_EnvironmentSystemSettings.h"
#include "XD_EnvironmentPreviewActor.h"
#include <EngineUtils.h>
#include "SubManager/XD_InsideManagerActor.h"


// Sets default values for this component's properties
UXD_EnvironmentManager::UXD_EnvironmentManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	SetIsReplicated(true);
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
}

void UXD_EnvironmentManager::OnUnregister()
{
	Super::OnUnregister();
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
}

void UXD_EnvironmentManager::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UXD_EnvironmentManager, Humidity);
	DOREPLIFETIME(UXD_EnvironmentManager, Temperature);
	DOREPLIFETIME(UXD_EnvironmentManager, WindVelocity);
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

	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
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
