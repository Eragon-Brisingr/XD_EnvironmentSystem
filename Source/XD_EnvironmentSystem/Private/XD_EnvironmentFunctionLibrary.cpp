// Fill out your copyright notice in the Description page of Project Settings.

#include "XD_EnvironmentFunctionLibrary.h"
#include "XD_EnvironmentManager.h"




class UXD_EnvironmentManager* UXD_EnvironmentFunctionLibrary::GetEnvironmentManager(const UObject* WorldContextObject)
{
	return UXD_EnvironmentManager::GetManager(WorldContextObject);
}

float UXD_EnvironmentFunctionLibrary::GetHumidity(const UObject* WorldContextObject, const FVector& Position)
{
	if (UXD_EnvironmentManager* EnvironmentManager = GetEnvironmentManager(WorldContextObject))
	{
		return EnvironmentManager->GetHumidity(Position);
	}
	return {};
}

float UXD_EnvironmentFunctionLibrary::GetTemperature(const UObject* WorldContextObject, const FVector& Position)
{
	if (UXD_EnvironmentManager* EnvironmentManager = GetEnvironmentManager(WorldContextObject))
	{
		return EnvironmentManager->GetTemperature(Position);
	}
	return {};
}

FVector UXD_EnvironmentFunctionLibrary::GetWindVelocity(const UObject* WorldContextObject, const FVector& Position)
{
	if (UXD_EnvironmentManager* EnvironmentManager = GetEnvironmentManager(WorldContextObject))
	{
		return EnvironmentManager->GetWindVelocity(Position);
	}
	return {};
}

float UXD_EnvironmentFunctionLibrary::GetCloudsDensity(const UObject* WorldContextObject, const FVector& Position)
{
	if (UXD_EnvironmentManager* EnvironmentManager = GetEnvironmentManager(WorldContextObject))
	{
		return EnvironmentManager->GetCloudsDensity(Position);
	}
	return {};
}

float UXD_EnvironmentFunctionLibrary::GetFogDensity(const UObject* WorldContextObject, const FVector& Position)
{
	if (UXD_EnvironmentManager* EnvironmentManager = GetEnvironmentManager(WorldContextObject))
	{
		return EnvironmentManager->GetFogDensity(Position);
	}
	return {};
}

float UXD_EnvironmentFunctionLibrary::GetRainfall(const UObject* WorldContextObject, const FVector& Position)
{
	if (UXD_EnvironmentManager* EnvironmentManager = GetEnvironmentManager(WorldContextObject))
	{
		return EnvironmentManager->GetRainfall(Position);
	}
	return {};
}

float UXD_EnvironmentFunctionLibrary::GetSnowfall(const UObject* WorldContextObject, const FVector& Position)
{
	if (UXD_EnvironmentManager* EnvironmentManager = GetEnvironmentManager(WorldContextObject))
	{
		return EnvironmentManager->GetSnowfall(Position);
	}
	return {};
}
