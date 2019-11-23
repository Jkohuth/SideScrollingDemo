// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelCameraFollowBounds.generated.h"

UCLASS()
class SIDESCROLLINGDEMO_API ALevelCameraFollowBounds : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelCameraFollowBounds();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USceneComponent* BoundsParent;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* Bounds0;

	UPROPERTY(VisibleAnywhere)
	TArray<class UBoxComponent*> LevelBounds;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void InitializeComponent();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
