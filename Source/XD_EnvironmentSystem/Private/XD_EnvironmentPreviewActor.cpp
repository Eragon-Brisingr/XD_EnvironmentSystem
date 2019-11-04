// Fill out your copyright notice in the Description page of Project Settings.

#include "XD_EnvironmentPreviewActor.h"
#include "XD_EnvironmentManager.h"


// Sets default values
AXD_EnvironmentPreviewActor::AXD_EnvironmentPreviewActor(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsEditorOnlyActor = true;

	PreviewEnvironmentManager = CreateDefaultSubobject<UXD_EnvironmentManager>(GET_MEMBER_NAME_CHECKED(AXD_EnvironmentPreviewActor, PreviewEnvironmentManager));
}

// Called when the game starts or when spawned
void AXD_EnvironmentPreviewActor::BeginPlay()
{
	Super::BeginPlay();
	Destroy();
}

void AXD_EnvironmentPreviewActor::Destroyed()
{
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);
	for (AActor* AttachedActor : AttachedActors)
	{
		AttachedActor->Destroy();
	}
}

#if WITH_EDITOR
void AXD_EnvironmentPreviewActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AXD_EnvironmentPreviewActor, Fresh))
	{
		Fresh = false;
		for (UXD_EnvironmentSubManager* SubManager : PreviewEnvironmentManager->SubManagers)
		{
			if (SubManager)
			{
				SubManager->ReceiveTick(10000000.f);
				SubManager->WhenFreshClick();
			}
		}
	}
	else 
	{
		for (UXD_EnvironmentSubManager* SubManager : PreviewEnvironmentManager->SubManagers)
		{
			if (SubManager && GetOwner())
			{
				SubManager->ReceiveOnConstruction(GetOwner()->GetActorTransform());
			}
		}
	}
}

bool AXD_EnvironmentPreviewActor::ShouldTickIfViewportsOnly() const
{
	if (UWorld* World = GetWorld())
	{
		return World->WorldType == EWorldType::Editor && !Pause;
	}
	return false;
}

#endif // WITH_EDITOR

void AXD_EnvironmentPreviewActor::TickActor(float DeltaSeconds, ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	FEditorScriptExecutionGuard ScriptGuard;
	Super::TickActor(DeltaSeconds, TickType, ThisTickFunction);
}

// Called every frame
void AXD_EnvironmentPreviewActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AXD_EnvironmentPreviewActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

