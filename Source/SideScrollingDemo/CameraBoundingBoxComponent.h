// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraBoundingBoxComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDESCROLLINGDEMO_API UCameraBoundingBoxComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCameraBoundingBoxComponent();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CameraBounds", meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CameraBounds", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* BoundingBox;
	
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	FVector BoxSize = FVector(50.f, 300.f, 3000.f);

	FVector Origin; // This is just origin
	FVector BoxExtent; // This should exist somewhere in here

	float right, left, top, bottom;

	FVector TargetLocation;
	float halfHeight, radius;

	float targetLeft, targetRight, targetTop, targetBottom;

	UFUNCTION(BlueprintCallable)
		void InitializePosition(UCapsuleComponent* targetCapsule);
	UFUNCTION(BlueprintCallable)
		void UpdatePosition(UCapsuleComponent* targetCapsule);
	UFUNCTION(BlueprintCallable)
		void ResetCamera(AActor* targetActor);

	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	FORCEINLINE class UBoxComponent* GetBoundingBox() const { return BoundingBox; }

		
};
