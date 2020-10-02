// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "XD_EnvironmentSkySphereBase.generated.h"

UCLASS(abstract)
class XD_ENVIRONMENTSYSTEM_API AXD_EnvironmentSkySphereBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AXD_EnvironmentSkySphereBase();

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

public:	
	// Called every frame
	void Tick(float DeltaTime) override;

#if WITH_EDITOR
	bool ShouldTickIfViewportsOnly() const override;

	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	void TickActor(float DeltaSeconds, ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;

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

