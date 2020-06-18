// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SideScrollingDemo.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, SideScrollingDemo, "SideScrollingDemo" );
//IMPLEMENT_PRIMARY_GAME_MODULE(FSideScrollingDemoModule, SideScrollingDemo, "SideScrollingDemo");
DEFINE_LOG_CATEGORY(LogSideScrollingDemo);

void FSideScrollingDemoModule::StartupModule() {
#if (ENGINE_MINOR_VERSION >= 21)    
	FString ShaderDirectory = FPaths::Combine(FPaths::ProjectDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping("/Project", ShaderDirectory);
#endif

}

void FSideScrollingDemoModule::ShutdownModule()
{
	ResetAllShaderSourceDirectoryMappings();
}

