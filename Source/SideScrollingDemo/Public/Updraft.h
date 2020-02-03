// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Updraft.generated.h"

UCLASS()
class SIDESCROLLINGDEMO_API AUpdraft : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUpdraft();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* UpdraftVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* Draft;

	UPROPERTY(BlueprintReadWrite)
	float strongDraftSpeed;
	UPROPERTY(BlueprintReadWrite)
	float weakDraftSpeed;

	float strongDraftThreshold;
	float weakDraftThreshold;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float GetDraftSpeed(FVector ActorLocation, float GravityScalar);


	FORCEINLINE class UBoxComponent* GetUpdraftVolume() const { return UpdraftVolume;  }
	FORCEINLINE class UParticleSystem* GetDraft() const { return Draft; }

};
