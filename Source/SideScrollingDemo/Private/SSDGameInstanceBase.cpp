// Fill out your copyright notice in the Description page of Project Settings.


#include "SSDGameInstanceBase.h"
#include "SSDSaveGame.h"
#include "Kismet/GameplayStatics.h"

USSDGameInstanceBase::USSDGameInstanceBase()
	: SaveSlot(TEXT("SaveGame"))
	, SaveUserIndex(0)
{}


USSDSaveGame* USSDGameInstanceBase::GetCurrentSaveGame() {
	return CurrentSaveGame;
}

void USSDGameInstanceBase::SetSavingEnabled(bool bEnabled) {
	bSavingEnabled = bEnabled;
}
bool USSDGameInstanceBase::LoadOrCreateSaveGame() {

	CurrentSaveGame = nullptr;

	if (UGameplayStatics::DoesSaveGameExist(SaveSlot, SaveUserIndex) && bSavingEnabled)
	{
		CurrentSaveGame = Cast<USSDSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlot, SaveUserIndex));

		return true;
	}
	return false;

}

bool USSDGameInstanceBase::WriteSaveGame() {
	if (bSavingEnabled) {
		return UGameplayStatics::SaveGameToSlot(GetCurrentSaveGame(), SaveSlot, SaveUserIndex);
	}
	return false;
}

void USSDGameInstanceBase::ResetSaveGame() {
	bool bWasSavingEnabled = bSavingEnabled;
	bSavingEnabled = false;
	LoadOrCreateSaveGame();
	bSavingEnabled = bWasSavingEnabled;
}