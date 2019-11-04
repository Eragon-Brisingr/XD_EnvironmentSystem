// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "XD_EnvironmentGameStateInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UXD_Environment_GameStateInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class XD_ENVIRONMENTSYSTEM_API IXD_Environment_GameStateInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, Category = "游戏|环境系统")
	class UXD_EnvironmentManager* GetEnvironmentManager() const;
	virtual class UXD_EnvironmentManager* GetEnvironmentManager_Implementation() const { return nullptr; }
};
