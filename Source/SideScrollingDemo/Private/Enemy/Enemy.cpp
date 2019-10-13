// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Constants.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->InitCapsuleSize(10.f, 40.f);
	RootComponent = GetCapsuleComponent();

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	BoxComponent->InitBoxExtent(FVector(10.0f, 40.0f, 40.0f));
	
	//BoxComponent->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	//BoxComponent->SetWorldLocation(FVector(0.0f));
	//BoxComponent->SetCanEverAffectNavigation(false);
	//BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//BoxComponent->SetCollisionObjectType(COLLISION_ENEMY);
	//BoxComponent->SetCollisionResponseToChannel(COLLISION_PLAYER, ECollisionResponse::ECR_Block);
	BoxComponent->SetupAttachment(RootComponent);
	BoxComponent->OnComponentHit.AddDynamic(this, &AEnemy::OnHit);

	//GetArrowComponent()->SetupAttachment(RootComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnemyMesh"));
	MeshComponent->SetupAttachment(BoxComponent);

	//MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (GetCharacterMovement()) {
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		GetCharacterMovement()->UpdatedComponent = RootComponent;
	}
	else {
		UE_LOG(LogClass, Log, TEXT("Did not load"));
	}
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}
void AEnemy::PostInitializeComponents() {
	Super::PostInitializeComponents();

}
void AEnemy::MoveRight(float Value) {
	AddMovementInput(FVector(0.f, -1.f, 0.f), Value);
}
// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetCharacterMovement() != NULL && GetCharacterMovement()->IsMovingOnGround()) {
		UE_LOG(LogClass, Log, TEXT("The Enemy is moving on the ground"));
	}

}

void AEnemy::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {

}

void AEnemy::WasAttacked() {
	this->DisableActor();
}
void AEnemy::DisableActor() {
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

}