// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SideScrollingDemoGameMode.generated.h"

UCLASS(minimalapi)
class ASideScrollingDemoGameMode : public AGameModeBase
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere)
	bool isFocus;
	UPROPERTY(VisibleAnywhere)
	FTimerHandle timerFocus;
public:
	ASideScrollingDemoGameMode();

	UFUNCTION(BlueprintCallable)
	void SetisFocus(bool input);

	UFUNCTION(BlueprintCallable)
	bool GetisFocus();
	void LogFocus();
	UFUNCTION(BlueprintCallable)
		void ToggleTimerFocus(bool input);
};



