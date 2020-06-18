// Fill out your copyright notice in the Description page of Project Settings.


#include "WalkingPath.h"
#include "Components/SplineComponent.h"
#include "Components/BoxComponent.h"
#include "SideScrollingDemoTypes.h"
// Sets default values
AWalkingPath::AWalkingPath()
{

	// It might be worth considering thata single box stretched over the entire length of the path would work better

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SplinePath = CreateDefaultSubobject<USplineComponent>(TEXT("Path Spline"));
	TrailPath0 = CreateDefaultSubobject<UBoxComponent>(TEXT("Path0"));
	TrailPath1 = CreateDefaultSubobject<UBoxComponent>(TEXT("Path1"));

	//TrailPath0->AttachToComponent(SplinePath);
	TrailPath0->SetVisibility(true);
	TrailPath0->bHiddenInGame = false;
	//TrailPath0->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	//TrailPath1->AttachToComponent(SplinePath);
	TrailPath1->SetVisibility(true);
	TrailPath1->bHiddenInGame = false;
	//TrailPath1->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	TrailPath0->ComponentTags.Add(ECustomTags::PathTag);
	TrailPath1->ComponentTags.Add(ECustomTags::PathTag);
}
void AWalkingPath::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Super::BeginPlay();
	TrailPath0->SetWorldScale3D(FVector(.25f, 2.f, 3.f));
	TrailPath1->SetWorldScale3D(FVector(.25f, 2.f, 3.f));

	// Later can set this up to have multiple exits with a for loop
	FVector TrailPath1Location = SplinePath->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::Local);
	TrailPath1->SetRelativeLocation(TrailPath1Location);
}
// Called when the game starts or when spawned
void AWalkingPath::BeginPlay()
{
	
}

// Called every frame
void AWalkingPath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

