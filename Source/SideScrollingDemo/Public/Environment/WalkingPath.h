// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SideScrollingDemoTypes.h"
#include "WalkingPath.generated.h"

UCLASS()
class SIDESCROLLINGDEMO_API AWalkingPath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWalkingPath();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Path)
		class USplineComponent* SplinePath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Path)
		class UBoxComponent* TrailPath0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Path)
		class UBoxComponent* TrailPath1;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents();
	FORCEINLINE class USplineComponent* GetSplineComponent() const { return SplinePath; }
	FORCEINLINE class UBoxComponent* GetTrailPath0() const { return TrailPath0;  }
	FORCEINLINE class UBoxComponent* GetTrailPath1() const { return TrailPath1;  }
};
