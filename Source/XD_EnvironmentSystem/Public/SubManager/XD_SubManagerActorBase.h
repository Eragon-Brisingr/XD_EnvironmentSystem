// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameFramework/Actor.h>
#include "XD_SubManagerActorBase.generated.h"

/**
 * 
 */
UCLASS(abstract)
class XD_ENVIRONMENTSYSTEM_API AXD_SubManagerActorBase : public AActor
{
	GENERATED_BODY()
public:
#if WITH_EDITOR
	virtual bool ShouldTickIfViewportsOnly() const override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void TickActor(float DeltaSeconds, ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "控制", meta = (DisplayName = "刷新"))
	uint8 Fresh : 1;

	UPROPERTY(EditAnywhere, Category = "控制", meta = (DisplayName = "暂停"))
	uint8 Pause : 1;
#endif

	UFUNCTION(BlueprintNativeEvent, Category = "控制")
	void WhenFreshClicked();
	void WhenFreshClicked_Implementation() {}
};
