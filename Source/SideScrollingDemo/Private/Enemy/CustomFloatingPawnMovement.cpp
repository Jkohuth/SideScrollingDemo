// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomFloatingPawnMovement.h"
#include "Constants.h"


UCustomFloatingPawnMovement::UCustomFloatingPawnMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bConstrainToPlane = true;
	SetPlaneConstraintAxisSetting(LockXAxis);
}
