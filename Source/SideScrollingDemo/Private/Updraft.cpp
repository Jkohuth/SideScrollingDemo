// Fill out your copyright notice in the Description page of Project Settings.


#include "Updraft.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystem.h"

// Sets default values
AUpdraft::AUpdraft()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UpdraftVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Volume"));
	UpdraftVolume->SetBoxExtent(FVector(50.f, 100.f, 300.f));
	RootComponent = UpdraftVolume;

	Draft = CreateDefaultSubobject<UParticleSystem>(TEXT("Particle"));
	
	strongDraftSpeed = 1450.f;
	weakDraftSpeed = 98000.f;



}

// Called when the game starts or when spawned
void AUpdraft::BeginPlay()
{
	Super::BeginPlay();
	FString tmp = UpdraftVolume->GetScaledBoxExtent().ToCompactString();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, tmp);

	strongDraftThreshold = GetActorLocation().Z - UpdraftVolume->GetScaledBoxExtent().Z;
	weakDraftThreshold = GetActorLocation().Z + (UpdraftVolume->GetScaledBoxExtent().Z / 2);

}

// Called every frame
void AUpdraft::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AUpdraft::GetDraftSpeed(FVector ActorLocation, float GravityScalar) {
	float ret;
	if (ActorLocation.Z >= strongDraftThreshold) {
		ret = strongDraftSpeed;
	}
	else if (ActorLocation.Z >= weakDraftThreshold) {
		ret = weakDraftSpeed;
	}
	ret *= GravityScalar;
	return ret;
}