// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SideScrollingDemoTypes.h"
#include "SSDCharacter.generated.h"

UCLASS()
class SIDESCROLLINGDEMO_API ASSDCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASSDCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Custom Movement to make game unique
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta (AllowPrivateAccess = "true"))
	class USSDPlayerMovementComponent* PlayerMovement;
	
	// What status is the character? Active, Dead, Cutscene
	UPROPERTY(Category = "Character State: CharacterState", BlueprintReadOnly)
	TEnumAsByte<enum ECharacterState> CharacterState;

	// Might make this a Camera Manager component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CameraBounds", meta = (AllowPrivateAccess = "true"))
	class UCameraBoundingBoxComponent* CameraBounds;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = Direction)
	class UArrowComponent* ArrowComponent;
	// Handle Axis Input
	void MoveRight(float Value);
	void MoveUp(float Value);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Where to implement C++ code when event triggers
	UFUNCTION()
	void OnActorOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnActorOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// JUMP - Platformer must have satisfying vertical movment
	virtual void Jump() override;
	virtual void StopJumping() override;
	virtual void NotifyJumpApex() override;

	// How long should the constant velocity be held for?
	UPROPERTY(EditAnywhere)
	float JumpTime = .22f;

	// This may be deleted later
	float countJumpTime = 0.f;
	bool isJumping = false;
	void JumpCalculated();
	void JumpActual();
	void SetupJumpCalculations();
	/////////////////////////////

	// DAMAGE

	UPROPERTY(EditAnywhere)
	float frameImmunity = 90.f;     // Duration of Immuity
	float currentDamageFrame = 0.f; // Counts frames
	bool immuneToDamage = false;		// Keeps track if immune to damage
	
	UPROPERTY(EditAnywhere)
	float Health = 3.0f;
	
	UPROPERTY(EditAnywhere)
	float FullHealth = 3.0;

	UFUNCTION(BlueprintCallable)
	void PostDamageImmunity(float DeltaTime); // Triggers Immunity

	// Handle Character Damage Inflicted
	UFUNCTION(BlueprintCallable)
	void InflictDamage(AActor* ImpactActor);

	// Handle Character Damage Taken
	UFUNCTION(BlueprintNativeEvent, Category = "Damage")
	float TakeDamage(float Damage, struct FPointDamageEvent const & DamageEvent, class AController *EventInstigator, AActor *DamageCauser);

	UFUNCTION(BlueprintImplementableEvent, Category = "Anim")
	void TriggerDeathAnim();

	UFUNCTION(BlueprintCallable, Category = "MalePlayer")
	void Respawn(FVector LastCheckPoint);

	// CHARACTER STATE
	UFUNCTION(BlueprintCallable, Category = "MalePlayer")
	virtual void SetCharacterState(ECharacterState NewCharacterState);
	UFUNCTION(BlueprintCallable, Category = "MalePlayer")
	virtual ECharacterState GetCharacterState();
	
	UFUNCTION(BlueprintCallable, Category = "Pawn|Character")
	bool IsGrinding() const;

	UFUNCTION(BlueprintCallable, Category = "Pawn|Character")
	bool IsClimbing() const;



	// BACK DASH - needs button delay
	UFUNCTION(BlueprintCallable, Category = "MalePlayer")
	void BackDash();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "MalePlayer")
	void BackDashTrigger();

	FORCEINLINE class USSDPlayerMovementComponent* GetPlayerMovement; () const { return PlayerMovement; }

	// SLIDE MOVEMENT

	UFUNCTION(BlueprintCallable, Category = "Pawn|Character")
		bool IsSliding() const;

	bool WantsToSlide() const;

	// Event called when player presses slide button
	void OnStartSlide();
	// Event called when player releases slide button
	void OnStopSlide();

	FORCEINLINE class UArrowComponent* GetArrowComponent const { return ArrowComponent; }


private:
	uint32 bPressedSlide:1;
};
