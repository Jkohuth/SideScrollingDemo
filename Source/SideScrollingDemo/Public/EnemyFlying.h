// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyFlying.generated.h"

UCLASS()
class SIDESCROLLINGDEMO_API AEnemyFlying : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyFlying(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = Movement, meta = (AllowPrivateAccess = "true"))
		class UEnemyFlyingMovementComponent* EnemyFlyingMovement;


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Landed(const FHitResult& Hit);

	bool playerSpotted = false;
	float Health = 1.f;

	UPROPERTY(BlueprintReadWrite)
		FVector OscillateMovement;


	// Damage Handlers
	UFUNCTION()
		float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser);
	UFUNCTION()
		void InflictDamage(AActor* ImpactActor, const FHitResult& Hit);

	// Collision Handlers
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(BlueprintReadWrite)
		FTimerHandle isRenderedTimer;
	UFUNCTION(BlueprintCallable)
		bool isRenderedToScreen(APlayerController *playerController);

	UPROPERTY(BlueprintReadWrite, Category = "Render")
		float checkScreenRendered = 0.5f;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Render")
		void TriggerScreenRenderedCheck();

	FORCEINLINE class UEnemyFlyingMovementComponent* GetEnemyFlyingMovement() const { return EnemyFlyingMovement;  }

};
