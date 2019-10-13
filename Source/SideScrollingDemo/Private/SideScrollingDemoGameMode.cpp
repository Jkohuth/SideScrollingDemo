// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SideScrollingDemoGameMode.h"
#include "SideScrollingDemoCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine.h"

ASideScrollingDemoGameMode::ASideScrollingDemoGameMode()
{
	// set default pawn class to our Blueprinted character
	// ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/MalePlayer_BP"));
	//if (PlayerPawnBPClass.Class != NULL)
	//{
	//	DefaultPawnClass = PlayerPawnBPClass.Class;
//	}
	isFocus = false; // The World won't start in this state
}

void ASideScrollingDemoGameMode::SetisFocus(bool input) {
	isFocus = input;
	UE_LOG(LogClass, Log, TEXT("Focus was set"));
}
bool ASideScrollingDemoGameMode::GetisFocus() {
	return isFocus;
}
void ASideScrollingDemoGameMode::ToggleTimerFocus(bool input) {
	if (input) {

		GetWorld()->GetTimerManager().SetTimer(timerFocus, this, &ASideScrollingDemoGameMode::LogFocus, 2.f, false);
		UE_LOG(LogClass, Log, TEXT("Timer set"));
	}
	else {
		GetWorld()->GetTimerManager().ClearTimer(timerFocus);
		UE_LOG(LogClass, Log, TEXT("Timer was cleared"));
	}
}
void ASideScrollingDemoGameMode::LogFocus() {
	UE_LOG(LogClass, Log, TEXT("It has been logged"));
}