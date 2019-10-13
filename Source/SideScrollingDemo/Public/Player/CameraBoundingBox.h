// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraBoundingBox.generated.h"

UCLASS()
class SIDESCROLLINGDEMO_API ACameraBoundingBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACameraBoundingBox();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* CameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Bounds, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* BoundingBox;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	FVector BoxSize = FVector(50.f, 150.f, 150.f);

	FVector Origin; // This is just origin
	FVector BoxExtent; // This should exist somewhere in here

	float right, left, top, bottom;

	FVector TargetLocation;
	float halfHeight, radius;

	float targetLeft, targetRight, targetTop, targetBottom;

	UFUNCTION(BlueprintCallable)
		void InitializePosition(AActor* targetActor);
	UFUNCTION(BlueprintCallable)
	void UpdatePosition(AActor* targetActor);
	UFUNCTION(BlueprintCallable)
		void ResetCamera(AActor* targetActor);
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	FORCEINLINE class UBoxComponent* GetBoundingBox() const { return BoundingBox; }
};
