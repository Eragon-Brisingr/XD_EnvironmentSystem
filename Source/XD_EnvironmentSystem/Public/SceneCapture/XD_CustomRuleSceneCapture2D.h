// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Components/SceneCaptureComponent2D.h>
#include "XD_CustomRuleSceneCapture2D.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class XD_ENVIRONMENTSYSTEM_API UXD_CustomRuleSceneCapture2D : public USceneCaptureComponent2D
{
	GENERATED_BODY()
	
public:
	UXD_CustomRuleSceneCapture2D();

protected:
	virtual void OnRegister() override;

	virtual void OnUnregister() override;

	FDelegateHandle OnActorSpawnedHandle;
public:
	UPROPERTY(EditAnywhere, Instanced, Category = "规则")
	class UXD_SceneCaptureRuleBase* SceneCaptureRule;
	
};

UCLASS(abstract, Blueprintable, EditInlineNew, collapsecategories)
class XD_ENVIRONMENTSYSTEM_API UXD_SceneCaptureRuleBase : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, Category = "规则")
	void ExecuteTheRule(class USceneCaptureComponent2D* SceneCapture2D, class AActor* Actor);
	virtual void ExecuteTheRule_Implementation(class USceneCaptureComponent2D* SceneCapture2D, class AActor* Actor){}
};

UCLASS()
class XD_ENVIRONMENTSYSTEM_API UXD_SceneCaptureRule_ObjectType : public UXD_SceneCaptureRuleBase
{
	GENERATED_BODY()
public:
	UXD_SceneCaptureRule_ObjectType();

	UPROPERTY(EditAnywhere, Category = "规则")
	TArray<TEnumAsByte<ECollisionChannel>> ObjectTypes;

	virtual void ExecuteTheRule_Implementation(class USceneCaptureComponent2D* SceneCapture2D, class AActor* Actor) override;
};
