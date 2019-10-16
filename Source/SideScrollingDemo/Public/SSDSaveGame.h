// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SSDSaveGame.generated.h"

/**
 * 
 */

namespace ESSDSaveGameVersion
{
	enum type
	{
		Initial,

		VersionPlusOne,
		LatestVersion = VersionPlusOne - 1
	};
}

UCLASS()
class SIDESCROLLINGDEMO_API USSDSaveGame : public USaveGame
{
	GENERATED_BODY()
	
	/* What is required to save in my Game?
		HP? No not worth it if I make save points areas to heal as well
		Locations In World> Yes <- Keep track of which level the player is at and which save point in the level
		Meaning Levels should have a list of save points

	*/
public:
	USSDSaveGame()
	{
		SavedDataVersion = ESSDSaveGameVersion::LatestVersion;
	}
protected:

	UPROPERTY()
	int32 SavedDataVersion = 0;

	/* Overridden */
	virtual void Serialize(FArchive& Ar) override;

};