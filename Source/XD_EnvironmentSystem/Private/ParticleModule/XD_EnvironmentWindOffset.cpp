// Fill out your copyright notice in the Description page of Project Settings.

#include "XD_EnvironmentWindOffset.h"
#include <Distributions/DistributionFloatUniform.h>
#include <ParticleEmitterInstances.h>
#include <Particles/ParticleEmitter.h>
#include "XD_EnvironmentManager.h"
#include <Particles/ParticleSystemComponent.h>




UXD_EnvironmentWindOffset::UXD_EnvironmentWindOffset()
{
	bUpdateModule = true;
}

void UXD_EnvironmentWindOffset::PostInitProperties()
{
	Super::PostInitProperties();
	if (!HasAnyFlags(RF_ClassDefaultObject | RF_NeedLoad))
	{
		if (!WindEffectWeight.IsCreated())
		{
			UDistributionFloatUniform* DistributionFloatUniform = NewObject<UDistributionFloatUniform>(this, TEXT("WindEffectWeight"));
			DistributionFloatUniform->Min = 50.f;
			DistributionFloatUniform->Max = 100.f;
			WindEffectWeight.Distribution = DistributionFloatUniform;
		}
	}
}

void UXD_EnvironmentWindOffset::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
	if ((Owner == NULL) || (Owner->ActiveParticles <= 0) ||
		(Owner->ParticleData == NULL) || (Owner->ParticleIndices == NULL))
	{
		return;
	}
	
	UParticleLODLevel* LODLevel = Owner->SpriteTemplate->GetCurrentLODLevel(Owner);
	check(LODLevel);
	FPlatformMisc::Prefetch(Owner->ParticleData, (Owner->ParticleIndices[0] * Owner->ParticleStride));
	FPlatformMisc::Prefetch(Owner->ParticleData, (Owner->ParticleIndices[0] * Owner->ParticleStride) + PLATFORM_CACHE_LINE_SIZE);
	FVector WindVelocity;
	if (UXD_EnvironmentManager* EnvironmentManager = UXD_EnvironmentManager::GetManager(Owner->Component))
	{
		WindVelocity = EnvironmentManager->GetWindVelocity(Owner->Component->GetComponentLocation());
	}
#if WITH_EDITOR
	else
	{
		WindVelocity = FVector(1.f, 0.f, 0.f) * DebugWindSpeed;
	}
#endif

	WindVelocity *= WindEffectWeight.GetValue(Owner->EmitterTime, Owner->Component) * DeltaTime;

	BEGIN_UPDATE_LOOP;
	{

		// 			FVector& UsedAcceleration = *((FVector*)(ParticleBase + CurrentOffset));
		// 			FVector TransformedUsedAcceleration = Mat.InverseTransformVector(UsedAcceleration);
		// 			FPlatformMisc::Prefetch(ParticleData, (ParticleIndices[i + 1] * ParticleStride));
		// 			FPlatformMisc::Prefetch(ParticleData, (ParticleIndices[i + 1] * ParticleStride) + PLATFORM_CACHE_LINE_SIZE);
		// 			Particle.Velocity += TransformedUsedAcceleration * DeltaTime;
		// 			Particle.BaseVelocity += TransformedUsedAcceleration * DeltaTime;

		Particle.Velocity += WindVelocity;
		Particle.BaseVelocity += WindVelocity;
	}
	END_UPDATE_LOOP;
}
