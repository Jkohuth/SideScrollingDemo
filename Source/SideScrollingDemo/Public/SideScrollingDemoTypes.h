// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"

#define COLLISION_ENEMY ECollisionChannel::ECC_GameTraceChannel6
#define COLLISION_PLAYER ECollisionChannel::ECC_GameTraceChannel4
#define COLLISION_WALL ECollisionChannel::ECC_GameTraceChannel8 // I believe this is outdated... Perhaps interactable?

#pragma region ENUMS

namespace EPlayerEffect
{

	enum ChargeMovement {
		UP, DOWN, LEFT, RIGHT, JUMP, ATK, NONE
	};


}
UENUM(BlueprintType)
enum ECustomMovementMode {
	MOVE_Wall, MOVE_Rail, MOVE_Path, MOVE_Grind, MOVE_Climb
};
UENUM(BlueprintType)
enum ECharacterState {
	ACTIVE, DEAD
};
UENUM(BlueprintType)
enum ENavigationType {
	MainLeft, MainRight
};

namespace ECustomTags {
	const FName RailTag = FName("rail");
	const FName WallTag = FName("wall");
	const FName PathTag = FName("path");
	const FName ClimbTag = FName("climb");
	const FName GrindTag = FName("grind");
	
	const FName BottomlessTag = FName("bottomless");
	
	// Later I may seperate the enemies from the movement modes
	const FName PlayerTag = FName("player");
	const FName EnemyTag = FName("enemy");
	const FName LevelBoundsTag = FName("levelbounds");
	const FName RestTag = FName("rest");


};

const EPlaneConstraintAxisSetting LockXAxis = EPlaneConstraintAxisSetting::X;