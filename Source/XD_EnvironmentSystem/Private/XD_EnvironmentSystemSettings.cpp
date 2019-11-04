// Fill out your copyright notice in the Description page of Project Settings.

#include "XD_EnvironmentSystemSettings.h"
#include "XD_EnvironmentPreviewActor.h"




UXD_EnvironmentSystemSettings::UXD_EnvironmentSystemSettings()
{
#if WITH_EDITOR
	EnvironmentPreviewActorClass = AXD_EnvironmentPreviewActor::StaticClass();
#endif
}
