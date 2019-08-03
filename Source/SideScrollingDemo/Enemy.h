// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class SIDESCROLLINGDEMO_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	void MoveRight(float Value);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
		class UStaticMeshComponent* MeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Box)
		class UBoxComponent* BoxComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void WasAttacked();
	UFUNCTION()
		void DisableActor();

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	FORCEINLINE class UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }
	FORCEINLINE class UBoxComponent* GetBoxComponent() const { return BoxComponent; }

};
