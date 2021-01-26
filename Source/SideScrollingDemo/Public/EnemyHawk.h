// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "SideScrollingDemoTypes.h"
#include "EnemyHawk.generated.h"

/**
 * 
 */
UCLASS()
class SIDESCROLLINGDEMO_API AEnemyHawk : public ADefaultPawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyHawk();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
#if WITH_EDITORONLY_DATA
	/** Component shown in the editor only to indicate character facing */
	UPROPERTY(BlueprintReadWrite)
		class UArrowComponent* ArrowComponent;
#endif


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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

};
