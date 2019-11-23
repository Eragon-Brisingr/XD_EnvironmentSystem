// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
//#include "UObject/NoExportTypes.h"
//#include "XD_EnvironmentSystemUtility.generated.h"

/**
 * 
 */
XD_ENVIRONMENTSYSTEM_API DECLARE_LOG_CATEGORY_EXTERN(XD_EnvironmentSystem_Log, Log, All);

#define EnvironmentSystem_Display_LOG(FMT, ...) UE_LOG(XD_EnvironmentSystem_Log, Display, TEXT(FMT), ##__VA_ARGS__)
#define EnvironmentSystem_Warning_LOG(FMT, ...) UE_LOG(XD_EnvironmentSystem_Log, Warning, TEXT(FMT), ##__VA_ARGS__)
#define EnvironmentSystem_Error_Log(FMT, ...) UE_LOG(XD_EnvironmentSystem_Log, Error, TEXT(FMT), ##__VA_ARGS__)

XD_ENVIRONMENTSYSTEM_API DECLARE_STATS_GROUP(TEXT("EnvironmentSystem"), STATGROUP_ENVIRONMENTSYSTEM, STATCAT_Advanced);