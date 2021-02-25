// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnemyFlyingMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class SIDESCROLLINGDEMO_API UEnemyFlyingMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

protected:
	UEnemyFlyingMovementComponent(const FObjectInitializer& ObjectInitializer);

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	virtual void BeginPlay() override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	
};
