// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "SideScrollingDemoTypes.h"
#include "CustomFloatingPawnMovement.generated.h"

/**
 * 
 */
UCLASS()
class SIDESCROLLINGDEMO_API UCustomFloatingPawnMovement : public UFloatingPawnMovement
{
	GENERATED_BODY()
	
protected:
	UCustomFloatingPawnMovement(const FObjectInitializer& ObjectInitializer);
	
	
};
