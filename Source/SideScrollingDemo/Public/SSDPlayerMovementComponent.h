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

	// Brute force it for now does it really matter if you prematurly optimize?
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

	void TriggerFocusMovement();
	void HaltFocusMovement();

	// Grind
	void PhysGrind(float DeltaTime, int32 Iterations);
	bool IsGrinding() const;
	void TriggerGrindMovement(USplineComponent* RailSpline, const FHitResult& RailCollision);
	float MaxGrindSpeed;
	float grindFriction = 1.f;
	float distanceAlongSpline;
	float MaxGrindAccel = 1000.f;
	float grindSpeed; // Speed not velocity it's only 2Dimensional 
	float slopeNormal;
	bool bJumpOffGrind = false;
	FVector UpdateLocation;
	FVector minGrindVelocity = FVector(0.f, 250.f, 0.f);

	// Swing
	void PhysSwing(float DeltaTime, int32 Iterations);
	//bool IsSwinging() const;
	void TriggerSwingMovement();

	void PrintStringToScreen(FString print);
private:
	uint32 bInSlide:1;
};
