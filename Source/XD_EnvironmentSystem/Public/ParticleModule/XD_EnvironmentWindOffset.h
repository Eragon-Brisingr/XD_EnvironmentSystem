// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Particles/Location/ParticleModuleLocationBase.h"
#include <Distributions/DistributionFloat.h>
#include "XD_EnvironmentWindOffset.generated.h"

/**
 * 
 */
UCLASS(editinlinenew, hidecategories = Object, meta = (DisplayName = "风偏移"))
class XD_ENVIRONMENTSYSTEM_API UXD_EnvironmentWindOffset : public UParticleModuleLocationBase
{
	GENERATED_BODY()
public:
	UXD_EnvironmentWindOffset();

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "风", meta = (DisplayName = "测试用风速"))
	float DebugWindSpeed = 0.1f;
#endif

	UPROPERTY(EditAnywhere, Category= "风", meta = (DisplayName = "受风影响权重"))
	struct FRawDistributionFloat WindEffectWeight;

	virtual void PostInitProperties() override;

	//~ Begin UParticleModule Interface
	//virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase) override;
	virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime) override;
	//~ Begin UParticleModule Interface
	
};
