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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CameraBounds", meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CameraBounds", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* BoundingBox;
	
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION()
		virtual void InitializeComponent();
	UFUNCTION()
	void OnBoundingBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnBoundingBoxOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UPROPERTY(EditAnywhere)
	FVector BoxSize = FVector(50.f, 300.f, 325.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform CameraTransform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FoV = 90.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BlendTime = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BlendExp = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 BlendFunc;

	void OnConstructionComponent();
	
	FVector Origin; // This is just origin
	FVector BoxExtent; // This should exist somewhere in here

	float right, left, top, bottom;

	FVector TargetLocation;
	float halfHeight, radius;

	float targetLeft, targetRight, targetTop, targetBottom;
	FVector shift;

	FVector CameraFollowLocation;
	FVector CameraFollowExtents;
	
	FVector CameraFollowLocationNext;
	FVector CameraFollowExtentsNext;
	FVector CameraFollowLocationPrevious;
	FVector CameraFollowExtentsPrevious;
	
	bool CheckLevelBounds();

	UFUNCTION(BlueprintCallable)
		void InitializePosition(APlayerController*  PlayerController, AActor* ActorInFocus, UCapsuleComponent* targetCapsule);
	UFUNCTION(BlueprintCallable)
		void UpdatePosition(UCapsuleComponent* targetCapsule);
	UFUNCTION(BlueprintCallable)
		void ResetCamera(AActor* targetActor);

	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	FORCEINLINE class UBoxComponent* GetBoundingBox() const { return BoundingBox; }

		
};
