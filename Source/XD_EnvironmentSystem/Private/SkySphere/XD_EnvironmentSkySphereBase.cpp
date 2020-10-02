// Fill out your copyright notice in the Description page of Project Settings.


#include "SkySphere/XD_EnvironmentSkySphereBase.h"

// Sets default values
AXD_EnvironmentSkySphereBase::AXD_EnvironmentSkySphereBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AXD_EnvironmentSkySphereBase::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AXD_EnvironmentSkySphereBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

#if WITH_EDITOR
bool AXD_EnvironmentSkySphereBase::ShouldTickIfViewportsOnly() const
{
	if (UWorld* World = GetWorld())
	{
		return World->WorldType != EWorldType::EditorPreview && !Pause;
	}
	return false;
}

void AXD_EnvironmentSkySphereBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AXD_EnvironmentSkySphereBase, Fresh))
	{
		Fresh = false;
		FEditorScriptExecutionGuard ScriptGuard;
		Tick(10000000.f);
	}
}

#endif

void AXD_EnvironmentSkySphereBase::TickActor(float DeltaSeconds, ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	FEditorScriptExecutionGuard ScriptGuard;
	Super::TickActor(DeltaSeconds, TickType, ThisTickFunction);
}
