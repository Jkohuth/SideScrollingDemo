// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "SoundDefinitions.h"
#include "SideScrollingDemoTypes.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSideScrollingDemo, Log, All);

class FSideScrollingDemoModule
	: public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};