// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SSDGameInstanceBase.generated.h"

/**
 * 
 */
UCLASS()
class SIDESCROLLINGDEMO_API USSDGameInstanceBase : public UGameInstance
{
	GENERATED_BODY()
	

public:
	// Constructor
	USSDGameInstanceBase();

	UPROPERTY(BlueprintReadWrite, Category = Save)
	FString SaveSlot;
	
	/** The platform-specific user index */
	UPROPERTY(BlueprintReadWrite, Category = Save)
		int32 SaveUserIndex;
	
	UFUNCTION(BlueprintCallable, Category = Save)
	class USSDSaveGame* GetCurrentSaveGame();
	
	UFUNCTION(BlueprintCallable, Category = Save)
	void SetSavingEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = Save)
	bool LoadOrCreateSaveGame();

	UFUNCTION(BlueprintCallable, Category = Save)
	bool WriteSaveGame();

	UFUNCTION(BlueprintCallable, Category = Save)
	void ResetSaveGame();


protected:
	UPROPERTY()
	USSDSaveGame* CurrentSaveGame;

	UPROPERTY()
	bool bSavingEnabled;

};
