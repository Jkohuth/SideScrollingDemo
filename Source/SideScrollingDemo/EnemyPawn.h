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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
		class UStaticMeshComponent* MeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Box)
		class UCapsuleComponent* CapsuleComponent;	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement)
		class UPawnMovementComponent* PawnMovement;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Awareness)
		class UPawnSensingComponent* PawnSensor;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
		float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser);
	UFUNCTION()
		void InflictDamage(AActor* ImpactActor, const FHitResult& Hit);
	UFUNCTION()
		void OnHearNoise(APawn *OtherActor, const FVector &Location, float Volume);
	UFUNCTION()
		void OnSeePawn(APawn *OtherPawn);
	float Health = 1.0f;
	//UFUNCTION()
	//	void WasAttacked();
	//UFUNCTION()
	//	void DisableActor();
	UFUNCTION()
		void Attack(APawn *OtherPawn);
	virtual UPawnMovementComponent* GetMovementComponent() const override;

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	FORCEINLINE class UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }
	FORCEINLINE class UCapsuleComponent* GetCapsuleComponent() const { return CapsuleComponent; }
};
