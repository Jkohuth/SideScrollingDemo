// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SSDPlayerMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class SIDESCROLLINGDEMO_API USSDPlayerMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

protected:
	float capsuleHalfHeight;
	float capsuleRadius;
	float capsuleRadius;
	float wallLinesCount;
	float wallLinesSpace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rail)
		class USplineComponent* RailSplineReference;

	
	TArray<bool> wallRayCheck;

public:

	// Standard Component functions
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

// This needs to be replaced by an arrow component
	FVector FacingDirection;
	void SetFacingDirection(FVector Value); // This could probably be a float but whatever

	// Controls passed to Movement Component
	void MoveRightInput(float Value);
	void MoveUpInput(float Value);

	
	UPROPERTY(EditAnywhere)
	float WallSlideFriction;

	// JUMP

	UPROPERTY(EditAnywhere)
	float RisingGravityScalar = 3.5f;
	UPROPERTY(EditAnywhere)
	float FallingGravityScalar = 8.f;

	bool isJumping = false;
	
	void SetCharacterGravity(float NewGravity);
	float JumpVelocity = 1750.f;
	UFUNCTION(BlueprintCallable)
	void OnJumpInput();
	
	UFUNCTION(BlueprintCallable)
	void BackDash();
	UFUNCTION(BlueprintCallable)
	void SetCustomMovementMode(uint8 custom);
	
	// DAMAGE

	UFUNCTION(BlueprintCallable)
	void KnockBack(const FHitResult& Hit);
	UPROPERTY(EditAnywhere)
	FVector KnockBackVelocity;
	
	// Climb
	void PhysClimb(float DeltaTime, int32 Iterations);
	bool IsClimbing() const;


	// Grind
	void PhysGrind(float DeltaTime, int32 Iterations);
	bool IsGrinding() const;

};
