// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SideScrollingDemoTypes.h"
#include "EnemyBird.generated.h"


UCLASS()
class SIDESCROLLINGDEMO_API AEnemyBird : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyBird();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = Enemy, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* MeshComponent; // Eventually will need to be static mesh
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = Enemy, meta=(AllowPrivateAccess = "true"))
		class USphereComponent* SphereComponent;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = Enemy, meta = (AllowPrivateAccess = "true"))
		class UArrowComponent* ArrowComponent;
	//UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = Enemy, meta = (AllowPrivateAccess = "true"))
	//	class UPawnSensingComponent* PawnSensor;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool playerSpotted = false;
	float Health = 1.f;

	// Probably a dangerous thing to do but....
	UPROPERTY(BlueprintReadWrite)
		APlayerController* MainPlayerController;

	
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

	//UPROPERTY(BlueprintAssignable)
	//	FEnemyRenderSignature OnEnemyRenderCheck;

	UPROPERTY(BlueprintReadWrite, Category = "Render")
		float checkScreenRendered = 0.5f;
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Render")
	void TriggerScreenRenderedCheck();

	FORCEINLINE class UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }
	FORCEINLINE class USphereComponent* GetSphereComponent() const { return SphereComponent; }

};
