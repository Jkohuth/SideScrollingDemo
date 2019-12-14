// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyWall.generated.h"

UCLASS()
class SIDESCROLLINGDEMO_API AEnemyWall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyWall();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	class UStaticMeshComponent* MeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Capsule)
	class UCapsuleComponent* CapsuleComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Direction)
	class UArrowComponent* ArrowComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spline)
	class USplineComponent* SplineComponent;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
		float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser);
	UFUNCTION(BlueprintCallable)
		void InflictDamage(AActor* ImpactActor, const FHitResult& Hit);

	
	FTimerHandle SenseTimer;

	float Health = 1.0f;
	TArray<FHitResult> ObjsInSight;
	
	//UPROPERTY(EditAnywhere)
	//float sensesPerSecond = 3.f;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void OnActorOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//UFUNCTION(BlueprintImplementableEvent, Category = "Event")
	//void OnSensePawn();


	FORCEINLINE class UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }
	FORCEINLINE class UCapsuleComponent* GetCapsuleComponent() const { return CapsuleComponent; }
	FORCEINLINE class USplineComponent* GetSplineComponent() const { return SplineComponent; }

};
