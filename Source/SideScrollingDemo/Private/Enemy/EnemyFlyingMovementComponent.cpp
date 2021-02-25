// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFlyingMovementComponent.h"
#include "SideScrollingDemoTypes.h"
#include "Engine.h"

UEnemyFlyingMovementComponent::UEnemyFlyingMovementComponent(const FObjectInitializer& ObjectInitializer) : UCharacterMovementComponent(ObjectInitializer)
{
	
}

void UEnemyFlyingMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UEnemyFlyingMovementComponent::BeginPlay() {
	Super::BeginPlay();

	//SetMovementMode(MOVE_Flying);
}

void UEnemyFlyingMovementComponent::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) {
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	FString movementModeStr = "Movement Mode Changed From ";
	switch(PrevMovementMode) {
		case EMovementMode::MOVE_Flying:
			// If it isn't flying it should be
			SetMovementMode(MOVE_Flying);
			movementModeStr.Append("MOVE_Flying");
			break;

		case EMovementMode::MOVE_Falling:
			movementModeStr.Append("MOVE_Falling");

			break;
		case EMovementMode::MOVE_Walking:
			movementModeStr.Append("MOVE_Walking");
			break;
		case EMovementMode::MOVE_None:
			movementModeStr.Append("MOVE_None");
			break;
		case EMovementMode::MOVE_Custom:
			movementModeStr.Append("MOVE_Custom");
			break;
		case EMovementMode::MOVE_Swimming:
			movementModeStr.Append("MOVE_Swimming");
			break;
		case EMovementMode::MOVE_NavWalking:
			movementModeStr.Append("MOVE_NavWalking");
			break;

	}
	movementModeStr.Append(" Current Movement Mode ");

	switch (MovementMode) {
		case EMovementMode::MOVE_Flying:
			// If it isn't flying it should be
			SetMovementMode(MOVE_Flying);
			movementModeStr.Append("MOVE_Flying");
			break;

		case EMovementMode::MOVE_Falling:
			movementModeStr.Append("MOVE_Falling");

			break;
		case EMovementMode::MOVE_Walking:
			movementModeStr.Append("MOVE_Walking");
			break;
		case EMovementMode::MOVE_None:
			movementModeStr.Append("MOVE_None");
			break;
		case EMovementMode::MOVE_Custom:
			movementModeStr.Append("MOVE_Custom");
			break;
		case EMovementMode::MOVE_Swimming:
			movementModeStr.Append("MOVE_Swimming");
			break;
		case EMovementMode::MOVE_NavWalking:
			movementModeStr.Append("MOVE_NavWalking");
			break;

	}
	
	
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, movementModeStr);

}