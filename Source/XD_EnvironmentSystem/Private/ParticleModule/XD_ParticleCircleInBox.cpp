// Fill out your copyright notice in the Description page of Project Settings.

#include "ParticleModule/XD_ParticleCircleInBox.h"

UXD_ParticleCircleInBox::UXD_ParticleCircleInBox()
{
	bUpdateModule = true;
	bSupported3DDrawMode = true;
}

void UXD_ParticleCircleInBox::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
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

	FVector WorldLocation = Owner->Component->GetComponentLocation();
	FVector2D WorldExtendsMax{ WorldLocation.X + Extends.X, WorldLocation.Y + Extends.Y };
	FVector2D WorldExtendsMin{ WorldLocation.X - Extends.X, WorldLocation.Y - Extends.Y };

	BEGIN_UPDATE_LOOP;
	{
		FVector& Location = Particle.Location;
		if (Location.X > WorldExtendsMax.X)
		{
			Location.X -= Extends.X * 2.f;
		}
		else if (Location.X < WorldExtendsMin.X)
		{
			Location.X += Extends.X * 2.f;
		}
		if (Location.Y > WorldExtendsMax.Y)
		{
			Location.Y -= Extends.Y * 2.f;
		}
		else if (Location.Y < WorldExtendsMin.Y)
		{
			Location.Y += Extends.Y * 2.f;
		}
	}
	END_UPDATE_LOOP;
}

void UXD_ParticleCircleInBox::Render3DPreview(FParticleEmitterInstance* Owner, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
#if WITH_EDITOR
	DrawOrientedWireBox(PDI, FVector::ZeroVector, FVector::ForwardVector, FVector::RightVector, FVector::UpVector, FVector(Extends.X, Extends.Y, 1000000.f), FColor::Yellow, SDPG_World);
#endif
}
