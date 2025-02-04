// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "Delegates/Delegate.h"

#define COLLISION_ENEMY ECollisionChannel::ECC_GameTraceChannel6
#define COLLISION_PLAYER ECollisionChannel::ECC_GameTraceChannel4
#define COLLISION_WALL ECollisionChannel::ECC_GameTraceChannel8 // I believe this is outdated... Perhaps interactable?

#pragma region ENUMS

DECLARE_DELEGATE
(
	FRenderedDelegate
)

namespace EPlayerEffect
{

	enum ChargeMovement {
		UP, DOWN, LEFT, RIGHT, JUMP, ATK, NONE
	};


}
UENUM(BlueprintType)
enum ECustomMovementMode {
	MOVE_Climb, MOVE_Grind, MOVE_Swing, MOVE_Path
};
UENUM(BlueprintType)
enum ECharacterState {
	ACTIVE, DEAD, REST
};
UENUM(BlueprintType)
enum ENavigationType {
	MainLeft, MainRight
};
UENUM(BlueprintType)
enum ECameraMode {
	MAIN, CAVE
};

namespace ECustomTags {
	const FName RailTag = FName("rail");
	const FName WallTag = FName("wall");
	const FName PathTag = FName("path");
	const FName ClimbTag = FName("climb");
	const FName GrindTag = FName("grind");
	const FName SwingTag = FName("swing");
	
	// Enivronment
	const FName BottomlessTag = FName("bottomless");
	const FName WindTag = FName("wind");
	const FName CaveTag = FName("cave");
	const FName EnvironmentDamageTag = FName("environmentdamage");
	// Later I may seperate the enemies from the movement modes
	const FName PlayerTag = FName("player");
	const FName EnemyTag = FName("enemy");
	const FName LevelBoundsTag = FName("levelbounds");
	const FName RestTag = FName("rest");
	const FName DeathTag = FName("death");


};

const EPlaneConstraintAxisSetting LockXAxis = EPlaneConstraintAxisSetting::X;