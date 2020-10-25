// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SSDPlayerMovementComponent.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FPlayerMovementHandler
{
	GENERATED_USTRUCT_BODY()
 
public:
	UPROPERTY(EditAnywhere, Category = "Movement")
	float AirControl;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float JumpZVelocity;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float GroundFriction;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxWalkSpeed;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxAcceleration; // Is actually used when calculating certain movementmodes

	// WallSlide
	UPROPERTY(EditAnywhere, Category = "Movement")
	float WallSlideJumpSpeed;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float WallSlideFriction;

	
	// Grind
	UPROPERTY(EditAnywhere, Category = "Movement")
	float GrindJumpSpeed;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float GrindMaxAccel;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float GrindMaxSpeed;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float GrindFriction;

	// Swing
	UPROPERTY(EditAnywhere, Category = "Movement")
	float SwingMaxAccel;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float SwingMaxSpeed;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float SwingFriction;
	
	FPlayerMovementHandler() {}

	FPlayerMovementHandler(float airControl, float jumpZVelocity, float groundFriction, float maxWalkSpeed, float maxAcceleration, float wallSlideJumpSpeed, float wallSlideFriction, 
		float grindJumpSpeed, float grindMaxAccel, float grindMaxSpeed, float grindFriction, float swingMaxAccel, float swingMaxSpeed, float swingFriction)
	{
		this->AirControl = airControl;
		this->JumpZVelocity = jumpZVelocity;
		this->GroundFriction = groundFriction;
		this->MaxWalkSpeed = maxWalkSpeed;
		this->MaxAcceleration = maxAcceleration;

		// Wall Slide (Climb is a bit of a misnomer)
		this->WallSlideJumpSpeed = wallSlideJumpSpeed;
		this->WallSlideFriction = wallSlideFriction;

		// Grind
		this->GrindJumpSpeed = grindJumpSpeed;
		this->GrindMaxAccel = grindMaxAccel;
		this->GrindMaxSpeed = grindMaxSpeed;
		this->GrindFriction = grindFriction;

		// Swing
		this->SwingMaxAccel = swingMaxAccel;
		this->SwingMaxSpeed = swingMaxSpeed;
		this->SwingFriction = swingFriction;
	}
};

UCLASS()
class SIDESCROLLINGDEMO_API USSDPlayerMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

protected:
	float capsuleHalfHeight;
	float capsuleRadius;
	float wallLinesCount;
	float wallLinesSpace;

	USSDPlayerMovementComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		class USplineComponent* RailSplineReference;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
		class AUpdraft* UpdraftReference;
	
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
	
	UFUNCTION(BlueprintCallable)
	void SetCustomMovementMode(uint8 CustomMovement);	
	UFUNCTION(BlueprintCallable)
	bool CheckCustomMovementMode(uint8 CustomMovement);

	UFUNCTION(BlueprintCallable)
		bool CheckMovementMode(uint8 ExpectedMovement);

	UPROPERTY(EditAnywhere)
	float WallSlideFriction;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxAccel = 3500.f;
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
	void JumpOffWall();
	UPROPERTY(EditAnywhere)
	FVector jumpWallVelocity;

	UFUNCTION(BlueprintCallable)
	FString GetMovementModeString();
	
	UFUNCTION(BlueprintCallable)
	void BackDash();
	
	FVector MaxMovementSpeeds;

	// DAMAGE

	UFUNCTION(BlueprintCallable)
	void KnockBack(const FHitResult& Hit);
	UPROPERTY(EditAnywhere)
	FVector KnockBackVelocity;
	
	// Slide
	bool IsSliding() const;

	// Climb
	UFUNCTION()
	void PhysClimb(float DeltaTime, int32 Iterations);
	bool IsClimbing() const;
	void TriggerClimbMovement(FHitResult ClimbTrigger);
	void ClimbCollisionHandler();
	float towardWall = 0.f;
	float climbCheckPadding = 10.f;
	

	// Need to clean this up later thinking a speed and direction vector
	float WallSlideJumpSpeed;
	UPROPERTY(EditAnywhere)
	FVector wallSlideVelocity;

	// Updraft

	bool bUpdraft = false;
	bool IsUpdrafting() const;
	void TriggerUpdraftMovement(AUpdraft* updraft);
	void HaltUpdraftMovement();
	void UpdateUpdraftMovement(float DeltaTime);
	float updraftSpeed;

	// Focus
	float GrindJumpSpeed;
	// Brute force it for now does it really matter if you prematurly optimize?
	UPROPERTY(EditAnywhere)				  	//AirControl, JumpZVelocity, GroundFriction, MaxWalkSpeed, MaxAcceleration, WallSlideJumpSpeed, WallSlideFriction, GrindJumpSpeed, GrindMaxAccel, GrindMaxSpeed, GrindFriction, SwingMaxAccel, SwingFriction
		FPlayerMovementHandler NormalPlayerMovement =		FPlayerMovementHandler(0.6f, 1750.f, 2.f, 1300.f, 3500.f, 1000.f, 3.f, 1000.f, 1000.f, 1500.f, 1.f, 1200.f, 1700.f, .4f);
	UPROPERTY(EditAnywhere)
		FPlayerMovementHandler NormalPlayerMovementAttack = FPlayerMovementHandler(0.2f, 1450.f, 2.f, 1000.f, 3500.f, 1000.f, 3.f, 1000.f, 1000.f, 1500.f, 1.f, 400.f, 1700.f, .8f);
	UPROPERTY(EditAnywhere)
		FPlayerMovementHandler FocusPlayerMovement =		FPlayerMovementHandler(1.2f, 2000.f, 0.f, 1600.f, 4250.f, 1000.f, 3.f, 1500.f, 1000.f, 1500.f, 1.f, 400.f, 1700.f, .8f);
	UPROPERTY(EditAnywhere)
		FPlayerMovementHandler FocusPlayerMovementAttack =  FPlayerMovementHandler(0.6f, 1600.f, 0.f, 1200.f, 4250.f, 1000.f, 3.f, 1500.f, 1000.f, 1500.f, 1.f, 400.f, 1700.f, .8f);

	UFUNCTION(BlueprintCallable)
		FPlayerMovementHandler GetNormalPlayerMovement() { return NormalPlayerMovement;  }
	UFUNCTION(BlueprintCallable)
		FPlayerMovementHandler GetNormalPlayerMovementAttack() { return NormalPlayerMovementAttack; }
	UFUNCTION(BlueprintCallable)
		FPlayerMovementHandler GetFocusPlayerMovement() { return FocusPlayerMovement; }
	UFUNCTION(BlueprintCallable)
		FPlayerMovementHandler GetFocusPlayerMovementAttack() { return FocusPlayerMovementAttack; }

	UFUNCTION(BlueprintCallable)
		void UpdateCharacterMovementValues(FPlayerMovementHandler newPlayerMovementValues);
	/*
	FPlayerMovementHandler(float airControl, float jumpZVelocity, float groundFriction, float maxWalkSpeed, float maxFlySpeed, float maxAcceleration, float wallSlideFriction, float jumpRailVelocity)
	{
		this->AirControl = airControl;
		this->JumpZVelocity = jumpZVelocity;
		this->GroundFriction = groundFriction;
		this->MaxWalkSpeed = maxWalkSpeed;
		this->MaxFlySpeed = maxFlySpeed;
		this->MaxAcceleration = maxAcceleration;
		this->WallSlideFriction = wallSlideFriction;
		this->JumpRailVelocity = jumpRailVelocity;
	}
	float NormAirControl = 0.6f;
	float NormJumpZVelocity = 1750.f;
	float NormGroundFriction = 8.f;
	float NormMaxWalkSpeed = 1300.f;
	float NormMaxFlySpeed = 600.f;
	float NormMaxAcceleration = 3500.f;
	float NormWallSlideFriction = 3.0f;

	// Should be within a struct but fuck you I'm tired
	float FocusAirControl = 1.5;
	float FocusJumpZVelocity = 2.0f;
	float FocusGroundFriction = .01f;
	float FocusMaxWalkSpeed = 2.0f;
	float FocusMaxAcceleration = 3.f;
	float FocusGravityScale = 2.0f;
*/
	void TriggerFocusMovement();
	void HaltFocusMovement();

	// Grind
	void PhysGrind(float DeltaTime, int32 Iterations);
	bool IsGrinding() const;
	void TriggerGrindMovement(class ARail* RailSpline, const FHitResult& RailCollision);
	float GrindMaxAccel = 1000.f;
	float GrindMaxSpeed;
	float GrindFriction = 1.f;
	float distanceAlongSpline;
	float grindSpeed; // Speed not velocity it's only 2Dimensional 
	float slopeNormal;
	bool bJumpOffGrind = false;
	float railRadius;
	FVector UpdateLocation;
	FVector minGrindVelocity = FVector(0.f, 500.f, 0.f);
	float GetDistanceAlongSpline(USplineComponent* SplineComponent);


	// Swing
	void PhysSwing(float DeltaTime, int32 Iterations);
	bool IsSwinging() const;
	bool isSwinging = false;
	//bool IsSwinging() const;
	UFUNCTION(BlueprintCallable)
	void TriggerSwingMovement(FVector pivotPosition);
	void HaltSwingMovement();
	FQuat actorRotationPreSwing;
	FVector CalculateCharacterPivotDistance(FVector pivotPosition);
	UPROPERTY(BlueprintReadOnly)
	FVector pivotPosition;
	float GetAngleForSwing(FVector DirectionVector);
	FVector NewSwingVelocity(const FVector& InitialVelocity, const FVector& Gravity, const FVector& Tension, float DeltaTime) const;
	int reduceLogging = 0.f;
	float lengthOfPendulum = 220.f;
	UPROPERTY(BlueprintReadOnly)
		float horizontalDistanceFromPivot;
	UPROPERTY(EditAnywhere)
	float SwingFriction = .8f;
	UPROPERTY(EditAnywhere)
	float SwingMaxSpeed = 2500.f;
	UPROPERTY(EditAnywhere)
	float SwingMaxAccel = 400.f;

	void PrintStringToScreen(FString print);

	int frameCount = 0.f;
	void LogAtReducedRate(FString Log, int ratePerFrames);

private:
	uint32 bInSlide:1;
};