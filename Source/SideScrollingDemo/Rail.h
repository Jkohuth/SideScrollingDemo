// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rail.generated.h"

UCLASS()
class SIDESCROLLINGDEMO_API ARail : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARail();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* CylinderMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rail)
		class USplineComponent* RailSpline;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rail)
	//	class UCapsuleComponent* RailCapsule;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rail)
	//	class USplineMeshComponent* RailMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	FVector SplineLocation;
	UFUNCTION(BlueprintCallable)
		FVector GetSplineLocation();
	FVector Direction;
	float numberOfLines;
	UPROPERTY(EditAnywhere)
		float DistanceBetweenLines;
	UPROPERTY(EditAnywhere)
	float heightOfRays;
	float length;
	float splineLength;
	float holder;
	float refreshRate;

	float heightAdd;
	float lengthAdd;
	FVector locationClosestToWorldLocation;
	FVector WorldLocaltion; // This one may just be used for drawing in a while group
	// Set Delayed Tick Function for the raycast, no need for overkill
	UFUNCTION(Category=Trigger, BlueprintCallable)
		void RailRayCast();


	//FORCEINLINE class UStaticMeshComponent* GetCylinderMesh() const { return CylinderMesh;  }
	FORCEINLINE class USplineComponent* GetRailSpline() const { return RailSpline; }
	//FORCEINLINE class USplineMeshComponent* GetRailMeshSpline() const { return RailMesh; }
	
};
