﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "XD_EnvironmentFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class XD_ENVIRONMENTSYSTEM_API UXD_EnvironmentFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "游戏|环境系统", meta = (WorldContext = "WorldContextObject", CompactNodeTitle = "Environment"))
	static class UXD_EnvironmentManager* GetEnvironmentManager(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "游戏|环境系统", meta = (WorldContext = "WorldContextObject"))
	static float GetHumidity(const UObject* WorldContextObject, const FVector& Position);

	UFUNCTION(BlueprintCallable, Category = "游戏|环境系统", meta = (WorldContext = "WorldContextObject"))
	static float GetTemperature(const UObject* WorldContextObject, const FVector& Position);

	UFUNCTION(BlueprintCallable, Category = "游戏|环境系统", meta = (WorldContext = "WorldContextObject"))
	static FVector GetWindVelocity(const UObject* WorldContextObject, const FVector& Position);

	UFUNCTION(BlueprintCallable, Category = "游戏|环境系统", meta = (WorldContext = "WorldContextObject"))
	static float GetCloudsDensity(const UObject* WorldContextObject, const FVector& Position);

	UFUNCTION(BlueprintCallable, Category = "游戏|环境系统", meta = (WorldContext = "WorldContextObject"))
	static float GetFogDensity(const UObject* WorldContextObject, const FVector& Position);

	UFUNCTION(BlueprintCallable, Category = "游戏|环境系统", meta = (WorldContext = "WorldContextObject"))
	static float GetRainfall(const UObject* WorldContextObject, const FVector& Position);

	UFUNCTION(BlueprintCallable, Category = "游戏|环境系统", meta = (WorldContext = "WorldContextObject"))
	static float GetSnowfall(const UObject* WorldContextObject, const FVector& Position);

};
