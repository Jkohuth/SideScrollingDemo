// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnemyPawn.generated.h"

UCLASS()
class SIDESCROLLINGDEMO_API AEnemyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyPawn();

protected:
	// I should have the desired code inherit from this class but for the time being this will just be the for the worm
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
		class UStaticMeshComponent* MeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Capsule)
		class UCapsuleComponent* CapsuleComponent;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		class UPawnMovementComponent* PawnMovement;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Awareness)
	//	class UPawnSensingComponent* PawnSensor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Direction)
		class UArrowComponent* ArrowComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		class USplineComponent* SplineComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
		float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser);
	UFUNCTION(BlueprintCallable)
		void InflictDamage(AActor* ImpactActor, const FHitResult& Hit);
	UFUNCTION()
		void OnCustomSense();
	UFUNCTION()
		void Nothing();
	FTimerHandle SenseTimer;
	//UFUNCTION()
	//	void OnHearNoise(APawn *OtherActor, const FVector &Location, float Volume);
	//UFUNCTION()
	//	void OnSeePawn(APawn *OtherPawn);
	float Health = 1.0f;
	TArray<FHitResult> ObjsInSight;
	float reducedTickRate;
	float tickCounter = 0;
	UPROPERTY(EditAnywhere)
	float sensesPerSecond = 3.f; // ie 20 ticks in 60 fps is 1/3 second
	//UFUNCTION()
	//	void WasAttacked();
	//UFUNCTION()
	//	void DisableActor();
	UFUNCTION()
		void Attack(APawn *OtherPawn);
	virtual UPawnMovementComponent* GetMovementComponent() const override;

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION(BlueprintImplementableEvent, Category = "Event")
		void OnSensePawn();


	FORCEINLINE class UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }
	FORCEINLINE class UCapsuleComponent* GetCapsuleComponent() const { return CapsuleComponent; }
	FORCEINLINE class UPawnMovementComponent* GetPawnMovement() const { return PawnMovement; }
	FORCEINLINE class USplineComponent* GetSplineComponent() const { return SplineComponent; }
};
