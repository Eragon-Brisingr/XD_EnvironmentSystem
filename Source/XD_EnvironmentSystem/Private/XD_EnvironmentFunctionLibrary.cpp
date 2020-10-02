// Fill out your copyright notice in the Description page of Project Settings.

#include "XD_EnvironmentFunctionLibrary.h"
#if WITH_EDITOR
#include <LevelEditorViewport.h>
#include <Editor/EditorEngine.h>
#include <Editor.h>
#endif
#include <Kismet/GameplayStatics.h>
#include <Camera/PlayerCameraManager.h>

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

FVector UXD_EnvironmentFunctionLibrary::GetCameraLocation(const UObject* WorldContextObject)
{
#if WITH_EDITOR
	if (UWorld* World = WorldContextObject->GetWorld())
	{
		if (!World->IsGameWorld() || GEditor->bIsSimulatingInEditor)
		{
			if (FLevelEditorViewportClient* client = static_cast<FLevelEditorViewportClient*>(GEditor->GetActiveViewport()->GetClient()))
			{
				return client->GetViewLocation();
			}
		}
	}
#endif
	if (APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(WorldContextObject, 0))
	{
		return CameraManager->GetCameraLocation();
	}
	return FVector::ZeroVector;
}

FRotator UXD_EnvironmentFunctionLibrary::GetCameraRotation(const UObject* WorldContextObject)
{
#if WITH_EDITOR
	if (UWorld* World = WorldContextObject->GetWorld())
	{
		if (!World->IsGameWorld() || GEditor->bIsSimulatingInEditor)
		{
			if (FLevelEditorViewportClient* Client = static_cast<FLevelEditorViewportClient*>(GEditor->GetActiveViewport()->GetClient()))
			{
				return Client->GetViewRotation();
			}
		}
	}
#endif
	if (APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(WorldContextObject, 0))
	{
		return CameraManager->GetCameraRotation();
	}
	return FRotator::ZeroRotator;
}

