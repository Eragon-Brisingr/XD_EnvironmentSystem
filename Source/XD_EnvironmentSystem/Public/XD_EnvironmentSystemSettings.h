// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "XD_EnvironmentSystemSettings.generated.h"

/**
 * 
 */
UCLASS(Config = "XD_EnvironmentSystemSetting", defaultconfig)
class XD_ENVIRONMENTSYSTEM_API UXD_EnvironmentSystemSettings : public UObject
{
	GENERATED_BODY()
	
public:
	UXD_EnvironmentSystemSettings();
	
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "环境系统", Config)
	TSoftClassPtr<class AXD_EnvironmentPreviewActor> EnvironmentPreviewActorClass;
#endif

	UPROPERTY(EditAnywhere, Category = "环境系统", Config)
	TSoftClassPtr<class AXD_InsideManagerActor> InsideManagerActorClass;

	UPROPERTY(EditAnywhere, Category = "环境系统", Config)
	TSoftClassPtr<class AXD_SnowPathManagerActorBase> SnowPathManagerActorClass;

	UPROPERTY(EditAnywhere, Category = "环境系统", Config)
	TArray<TSoftClassPtr<class AXD_SubManagerActorBase>> SubManagerActorClasses;
};
