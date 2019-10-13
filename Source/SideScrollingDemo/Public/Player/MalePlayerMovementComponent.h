// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SideScrollingDemoTypes.h"
#include "MalePlayerMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class SIDESCROLLINGDEMO_API UMalePlayerMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
protected:
	float capsuleHalfHeight;
	float capsuleRadius;
	float wallLinesCount;
	float wallLinesSpace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rail)
		class USplineComponent* RailSplineReference;

	
	TArray<bool> wallRayCheck;
	bool once = true;
public:

	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

	FVector FacingDirection;
	void SetFacingDirection(FVector Value); // This could probably be a float but whatever

	void MoveRightInput(float Value);
	void MoveUpInput(float Value);
	

	UPROPERTY(EditAnywhere)
		float WallSlideFriction;

	////////////////////////////////////
	// JUMP
	////////////////////////////////////
	UPROPERTY(EditAnywhere)
	float RisingGravityScalar = 3.5f;
	UPROPERTY(EditAnywhere)
	float FallingGravityScalar = 8.f;

	bool isJumping = false;

	void SetGravity(float NewGravity);
	float JumpVelocity = 1750.f;
	//void UpdateGravity();
	//virtual void NotifyJumpApex() override;
	//virtual void StopJumping() override;
	//virtual void DoJump() override;

	UFUNCTION(BlueprintCallable)
		void BackDash();
	UFUNCTION(BlueprintCallable)
		void OnJumpInput();
	UFUNCTION(BlueprintCallable)
		void SetCustomMovementMode(uint8 custom);
	UFUNCTION(BlueprintCallable)
		void KnockBack(const FHitResult& Hit);
	UPROPERTY(EditAnywhere)
		FVector KnockBackVelocity;
	////////////////////////////////////
	// WALL MOVEMENT
	////////////////////////////////////
	float wallLeapSpeed = 1000.0f;
	FVector wallJumpVelocity = FVector(0.0f, 1000.0f, 1500.0f);
	FVector wallSlideVelocity = FVector(0.0f, 0.0f, -1000.f);
	bool bSlidingDownWall;
	FTimerHandle SlideDelayTimerHandle;
	FVector wallDirection = FVector::ZeroVector;
	FVector WallCheckLocationStart;
	FVector WallCheckLocationEnd;
	void WallCollisionHandler();
	void TriggerWallMovement(FHitResult Hit);
	void CheckWallBehind();
	void PhysWall(float DeltaTime, int32 Iterations);

	void FallOffWall(float Value);
	void InitiateSlide(float DeltaTime);
	void JumpOffWall();
	void TurnOnSliding() { bSlidingDownWall = true; }
	bool CheckCustomMovementMode(uint8 CustomMode);

	/////////////////////////////////////////////////
	// PATH MOVEMENT - 
	/////////////////////////////////////////////////
	void PhysPath(float DeltaTime, int32 Iterations);
	void AttachToPath(USplineComponent* PathSpline);
	class USplineComponent* PathSplineRef;
	void ConfirmPathExit();

	///////////////////////////////////////////////
	// RAIL MOVEMENT - (Should really be grinding)
	///////////////////////////////////////////////
	/** Returns true if the character is in the 'Grinding' movement mode. */
	//UFUNCTION(BlueprintCallable, Category = "Pawn|Components|CharacterMovement")
	//bool IsGrinding() const;


	FVector AverageBetweenSplinePoints(FVector splinePoint1, FVector splinePoint2);
	float DistanceBetweenTwoVectors(FVector one, FVector two);
	float distanceAlongSpline;
	float splineSpeed;

	bool bJumpOffRail = false;

	float RailControl = 0.5f;
	float RailControlBoostMultiplier = 2.f;
	float RailControlBoostVelocityThreshold = 25.f;
	float RailFriction = 1.f;
	float MaxRailAccel = 1000.f;
	float MaxSplineSpeed = 1000.f;
	UFUNCTION(BlueprintCallable)
		void AttachToRail(USplineComponent* RailSpline);
	FVector RailSpeed;
	//float RailFriction= 3.0f;
	void PhysRail(float DeltaTime, int32 Iterations);
	///////////////////////////////////////////////
	// Focus Movement
	///////////////////////////////////////////////
	void AccumulateForce(FVector Force);
	FVector MaxMovementSpeeds;
};
/*
FORCEINLINE_DEBUGGABLE bool UMalePlayerMovementComponet::IsGrinding() const {
	if(MovementMode == MOVE_Custom && Move)
}
*/