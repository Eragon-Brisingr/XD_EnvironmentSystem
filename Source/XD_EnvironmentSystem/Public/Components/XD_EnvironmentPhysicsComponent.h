// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Components/ActorComponent.h>
#include "XD_EnvironmentPhysicsComponent.generated.h"

class UXD_EnvironmentManager;


UCLASS(ClassGroup=(Environment), meta=(BlueprintSpawnableComponent, DisplayName = "EnvironmentPhysics"))
class XD_ENVIRONMENTSYSTEM_API UXD_EnvironmentPhysicsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UXD_EnvironmentPhysicsComponent();

protected:
	void BeginPlay() override;
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(Transient)
	UXD_EnvironmentManager* EnvironmentManager;

	UPROPERTY(EditAnywhere, Category = "风")
	TArray<FVector> WindTestPoint = { FVector::ZeroVector };
	UPROPERTY(EditAnywhere, Category = "风")
	float WindEffectScale = 10000.f;
};
