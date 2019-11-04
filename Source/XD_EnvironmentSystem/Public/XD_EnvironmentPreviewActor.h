// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameFramework/Info.h>
#include "XD_EnvironmentPreviewActor.generated.h"

UCLASS(Blueprintable, NotBlueprintType, hidecategories = (Input, Movement, Collision, Rendering, "Utilities|Transformation", Actor, HLOD, Tags, LOD, AssetUserData, Cooking), showcategories = ("Input|MouseInput", "Input|TouchInput"))
class XD_ENVIRONMENTSYSTEM_API AXD_EnvironmentPreviewActor : public AInfo
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AXD_EnvironmentPreviewActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
public:
#if WITH_EDITOR
	virtual bool ShouldTickIfViewportsOnly() const override;
#endif
	virtual void TickActor(float DeltaSeconds, ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "环境系统", meta = (DisplayName = "刷新"))
	uint8 Fresh : 1;

	UPROPERTY(EditAnywhere, Category = "环境系统", meta = (DisplayName = "暂停"))
	uint8 Pause : 1;
#endif

	UPROPERTY(VisibleAnywhere, Category = "环境系统")
	class UXD_EnvironmentManager* PreviewEnvironmentManager;
	
};
