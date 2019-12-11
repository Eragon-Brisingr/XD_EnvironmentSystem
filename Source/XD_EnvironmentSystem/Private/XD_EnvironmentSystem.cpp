// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "XD_EnvironmentSystem.h"
#if WITH_EDITOR
#include <ISettingsModule.h>
#include <ISettingsSection.h>
#include "XD_EnvironmentSystemSettings.h"
#endif
#include <Interfaces/IPluginManager.h>
#include <ShaderCore.h>

#define LOCTEXT_NAMESPACE "FXD_EnvironmentSystemModule"

void FXD_EnvironmentSystemModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
#if WITH_EDITOR
	// register settings
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule != nullptr)
	{
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "XD_EnvironmentSystemSettings",
			LOCTEXT("XD_EnvironmentSystemSettings", "XD_EnvironmentSystemSettings"),
			LOCTEXT("XD_EnvironmentSystemSettingsDescription", "Configure the XD_EnvironmentSystemSettings plug-in."),
			GetMutableDefault<UXD_EnvironmentSystemSettings>()
		);
	}
#endif //WITH_EDITOR

	// Maps virtual shader source directory /Plugin/DatasmithContent to the plugin's actual Shaders directory.
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("XD_EnvironmentSystem"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/XD_EnvironmentSystem"), PluginShaderDir);
}

void FXD_EnvironmentSystemModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FXD_EnvironmentSystemModule, XD_EnvironmentSystem)