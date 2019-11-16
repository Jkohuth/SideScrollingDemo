// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelCameraFollowBounds.h"
#include "Components/BoxComponent.h"

// Sets default values
ALevelCameraFollowBounds::ALevelCameraFollowBounds()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoundsParent = CreateDefaultSubobject<USceneComponent>(TEXT("BoundsContainer"));
	RootComponent = BoundsParent;

	Bounds0 = CreateDefaultSubobject<UBoxComponent>(TEXT("Bounds"));
	Bounds0->InitBoxExtent(FVector(75.f, 400.f, 400.f));
	Bounds0->SetWorldLocation(FVector::ZeroVector);
	
	Bounds0->SetupAttachment(RootComponent);
	Bounds0->bHiddenInGame = false;
	Bounds0->bVisible = true;
	Bounds0->bEditableWhenInherited = true;
	LevelBounds.Add(Bounds0);
}
void ALevelCameraFollowBounds::InitializeComponent() {

}
// Called when the game starts or when spawned
void ALevelCameraFollowBounds::BeginPlay()
{
	Super::BeginPlay();

	FString sizeString = FString::FromInt(LevelBounds.Num()) + " " + Bounds0->GetScaledBoxExtent().ToCompactString();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, sizeString);

	
}

// Called every frame
void ALevelCameraFollowBounds::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

