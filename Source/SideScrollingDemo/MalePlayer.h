// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MalePlayer.generated.h"

UCLASS()
class SIDESCROLLINGDEMO_API AMalePlayer : public ACharacter
{
 	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMalePlayer(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*// Side view Camera //
	PROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* SideViewCameraComponent;
	// Camera boom position //
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class ACameraBoundingBox* BoundingBox;
	*/
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		class UMalePlayerMovementComponent* MalePlayerMovement;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MalePlayer")
	//	class ACameraBoundingBox* Camera;
	UPROPERTY(Category = "Character State: CharacterState", BlueprintReadOnly)
		TEnumAsByte<enum ECharacterState> CharacterState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CameraBounds", meta = (AllowPrivateAccess = "true"))
		class UCameraBoundingBoxComponent* CameraBounds;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CameraBounds", meta = (AllowPrivateAccess = "true"))
	//	class USceneComponent* SceneComponent;
	// Called for the side to side input //
	void MoveRight(float Value);
	// Called for the slide down wall component
	void MoveUp(float Value);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Moving this socket offset value will be important in the future
	FVector SocketOffset = FVector(0.f, 200.f, 150.f);

	UFUNCTION()
		void OnActorOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnActorOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void DebugString();

	virtual void NotifyJumpApex() override;
	virtual void Jump() override;
	virtual void StopJumping() override;
	UPROPERTY(EditAnywhere)
	float JumpTime = .22f;
	//FName RailTag = FName("rail");
	//FName WallTag = FName("wall");
	////////////////////////////////////
	// Focus Movement (Maybe Echo Movement)
	////////////////////////////////////
	// Group these up in a struct later
	UPROPERTY(EditAnywhere)
	float focusDilation = 0.1f; // How much to dilate time
	UPROPERTY(EditAnywhere)
	float focusDilationPlayer = 0.5f;
	float normalTime = 1.f;
	UPROPERTY(EditAnywhere)
	float focusRealTimeDilation = 1.5f; // Amount of real world seconds that need to pass

	UPROPERTY(EditAnywhere)
		float focusRadiusApex = 500.f;
	
	float focusRadius = 500.f;
	FVector focusInitialLocation;
	float increaseRadiusRate = 0.0f;
	float decreaseRadiusRate = .0f;
	float recoilTime = 1.0f; // This needs to be expressed in the ratio of actual time by dilated time
	float focusIncreaseRadiusTime = .5f; // Time till radius is finished expanding
	float focusTimeCount; // Keeps track of how much time has passed since activation
	bool isFocusing;
	FVector focusImpluse;
	FVector focusTimeCorrection;
	void ActivateFocus();
	void DeactivateFocus();
	void UpdateFocus(float DeltaTime);

	TArray<FHitResult> FocusSphereZone;

	FVector DifferenceInElevation;
	FVector InitialLocation;
	FVector DeltaX1 = FVector::ZeroVector;
	FVector DeltaX2 = FVector::ZeroVector;
	FVector Direction = FVector::ZeroVector;
	
	float countJumpTime = 0.f;
	bool isJumping = false;
	void JumpCalculated();
	void JumpActual();
	void SetupJumpCalculations();

	// Crude Damage Handler
	float frameImmunity = 90.f;
	float currentDamageFrame = 0.f;
	bool immuneDamage = false;
	UFUNCTION(BlueprintCallable)
		void PostDamageImmunity(float DeltaTime);

	// Camera System needs to be held by the Player as well

	// DAMAGE SYSTEM
	UPROPERTY(EditAnywhere)
		float Health = 3.0f;
	UPROPERTY(EditAnywhere)
		float FullHealth = 3.0;
	UFUNCTION(BlueprintCallable)
	void InflictDamage(AActor* ImpactActor);
	UFUNCTION(BlueprintNativeEvent, Category="Damage")
	float TakeDamage(float Damage, struct FPointDamageEvent const & DamageEvent, class AController *EventInstigator, AActor *DamageCauser);
	UFUNCTION(BlueprintImplementableEvent, category = "Anim")
		void TriggerDeathAnim();
	UPROPERTY(BlueprintReadWrite)
	bool isDead = false;
	UFUNCTION(BlueprintCallable, Category="MalePlayer")
	void Respawn(FVector LastCheckPoint);

	// This is better than a bunch of booleans to dictate behavior

	UFUNCTION(BlueprintCallable, Category = "MalePlayer")
		virtual void SetCharacterState(ECharacterState NewCharacterState);
	UFUNCTION(BlueprintCallable, Category = "MalePlayer")
		virtual ECharacterState GetCharacterState();

	/**Determines when collided with certain objects, will use Line Tracing in tandem*/
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns SideViewCameraComponent subobject **/
//	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
//	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	
	//FORCEINLINE class UStaticMeshComponent* GetCapsuleMesh() const { return CapsuleMesh; }

	FORCEINLINE class UMalePlayerMovementComponent* GetMalePlayerMovement() const { return MalePlayerMovement;  }
	
	
};

