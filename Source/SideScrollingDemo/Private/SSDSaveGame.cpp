// Fill out your copyright notice in the Description page of Project Settings.

#include "SSDSaveGame.h"
#include "SSDGameInstanceBase.h"

USSDSaveGame::USSDSaveGame() {
	SaveSlotName = TEXT("SSDSaveSlot");
	UserIndex = 0; 
}
void USSDSaveGame::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	
	SavedDataVersion = ESSDSaveGameVersion::LatestVersion;

}