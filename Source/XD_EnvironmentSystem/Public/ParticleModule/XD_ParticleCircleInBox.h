// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Particles/Location/ParticleModuleLocationBase.h"
#include "XD_ParticleCircleInBox.generated.h"

/**
 * 
 */
//给雨或者雪的效果使用，粒子超出边界框后会从另一边回来
UCLASS(editinlinenew, hidecategories = Object, meta = (DisplayName = "粒子循环边界"))
class XD_ENVIRONMENTSYSTEM_API UXD_ParticleCircleInBox : public UParticleModuleLocationBase
{
	GENERATED_BODY()
	
public:
	UXD_ParticleCircleInBox();

	UPROPERTY(EditAnywhere, Category = "设置")
	FVector2D Extends = FVector2D(50.f, 50.f);
	
	virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime) override;

	virtual void Render3DPreview(FParticleEmitterInstance* Owner, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
};
