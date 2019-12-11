// Fill out your copyright notice in the Description page of Project Settings.

#include "SceneCapture/XD_CustomRuleSceneCapture2D.h"
#include <EngineUtils.h>


// Sets default values
UXD_CustomRuleSceneCapture2D::UXD_CustomRuleSceneCapture2D()
{
	SceneCaptureRule = CreateDefaultSubobject<UXD_SceneCaptureRule_ObjectType>(GET_MEMBER_NAME_CHECKED(UXD_CustomRuleSceneCapture2D, SceneCaptureRule));

}

void UXD_CustomRuleSceneCapture2D::OnRegister()
{
	Super::OnRegister();

	if (SceneCaptureRule)
	{
#if WITH_EDITOR
		FEditorScriptExecutionGuard ScriptGuard;
#endif
		for (TActorIterator<AActor> It(GetWorld(), AActor::StaticClass()); It; ++It)
		{
			AActor* Actor = *It;
			if (!Actor->IsPendingKill())
			{
				SceneCaptureRule->ExecuteTheRule(this, Actor);
			}
		}

		OnActorSpawnedHandle = GetWorld()->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateLambda([this](AActor* Actor)
		{
#if WITH_EDITOR
			FEditorScriptExecutionGuard ScriptGuard;
#endif
			SceneCaptureRule->ExecuteTheRule(this, Actor);
		}));
	}
}

void UXD_CustomRuleSceneCapture2D::OnUnregister()
{
	Super::OnUnregister();
	GetWorld()->RemoveOnActorSpawnedHandler(OnActorSpawnedHandle);
}

UXD_SceneCaptureRule_ObjectType::UXD_SceneCaptureRule_ObjectType()
{
	ObjectTypes.Add(ECollisionChannel::ECC_WorldStatic);
}

void UXD_SceneCaptureRule_ObjectType::ExecuteTheRule(class USceneCaptureComponent2D* SceneCapture2D, class AActor* Actor)
{
	for (UActorComponent* ActorComponent : Actor->GetComponents())
	{
		if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(ActorComponent))
		{
			if (!ObjectTypes.Contains(PrimitiveComponent->GetCollisionObjectType()))
			{
				SceneCapture2D->HideComponent(PrimitiveComponent);
			}
		}
	}
}
