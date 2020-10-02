// Fill out your copyright notice in the Description page of Project Settings.

#include "SubManager/XD_SubManagerActorBase.h"



#if WITH_EDITOR
bool AXD_SubManagerActorBase::ShouldTickIfViewportsOnly() const
{
	if (UWorld* World = GetWorld())
	{
		return World->WorldType != EWorldType::EditorPreview && !Pause;
	}
	return false;
}

void AXD_SubManagerActorBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AXD_SubManagerActorBase, Fresh))
	{
		Fresh = false;
		FEditorScriptExecutionGuard ScriptGuard;
		Tick(10000000.f);
	}
}

#endif

void AXD_SubManagerActorBase::TickActor(float DeltaSeconds, ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	FEditorScriptExecutionGuard ScriptGuard;
	Super::TickActor(DeltaSeconds, TickType, ThisTickFunction);
}
