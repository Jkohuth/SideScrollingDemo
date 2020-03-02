// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SideScrollingDemoTypes.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class SIDESCROLLINGDEMO_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Awareness)
		class UPawnSensingComponent* PawnSensor;

public:	
	bool rotated = false;
	UPROPERTY()
		FVector OriginPos;
	UPROPERTY(EditAnywhere)
		float MaxWalkRadius = 1000.f;

	FTimerHandle AnticipationTimer;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	float lungeVelocity = 1000.f;
	UPROPERTY()
		float Health = 1.f;

	UFUNCTION()
		float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser);
	UFUNCTION()
		void InflictDamage(AActor* ImpactActor, const FHitResult& Hit);

	UFUNCTION()
		void DoAttack();
	UFUNCTION()
		void TriggerAttack();

	UFUNCTION(BlueprintImplementableEvent, Category = "Sense")
		void PlayerSensed();

	UFUNCTION()
		void TurnAround();
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpluse, const FHitResult& Hit);

	// PawnSensingComponent Delegates
	UFUNCTION()
		void OnHearNoise(APawn *OtherActor, const FVector &Location, float Volume);

	UFUNCTION()
		void OnSeePawn(APawn *OtherPawn);
	
};
