// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyPawn.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "CustomFloatingPawnMovement.h"
#include "Constants.h"

// Sets default values
AEnemyPawn::AEnemyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	BoxComponent->InitBoxExtent(FVector(10.0f, 40.0f, 40.0f));

	RootComponent = BoxComponent;

	BoxComponent->OnComponentHit.AddDynamic(this, &AEnemyPawn::OnHit);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnemyMesh"));
	MeshComponent->SetupAttachment(RootComponent);
	//MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PawnMovement = CreateDefaultSubobject<UCustomFloatingPawnMovement>(TEXT("CustomMovement"));
	PawnMovement->UpdatedComponent = RootComponent;

}
void AEnemyPawn::PostInitializeComponents() {
	Super::PostInitializeComponents();

	if (GetMovementComponent() && BoxComponent) {
		GetMovementComponent()->UpdateNavAgent(*this);
	}
}
// Called when the game starts or when spawned
void AEnemyPawn::BeginPlay()
{
	Super::BeginPlay();
	
}
UPawnMovementComponent* AEnemyPawn::GetMovementComponent() const {
	return PawnMovement;
}
// Called every frame
void AEnemyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyPawn::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {

}

void AEnemyPawn::WasAttacked() {
	this->DisableActor();
}
void AEnemyPawn::DisableActor() {
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

}