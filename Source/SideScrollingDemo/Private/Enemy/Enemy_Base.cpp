// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Base.h"
#include "Perception/PawnSensingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AEnemy_Base::AEnemy_Base()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensor = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Sensor"));
	PawnSensor->SensingInterval = .25f; // How often does pawn react
	PawnSensor->bOnlySensePlayers = false;
	PawnSensor->SetPeripheralVisionAngle(35.f);

	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetCharacterMovement()->JumpZVelocity = 400.f;

}

// Called when the game starts or when spawned
void AEnemy_Base::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemy_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy_Base::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
float AEnemy_Base::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) {
	return 0.f;
}
void AEnemy_Base::InflictDamage(AActor* ImpactActor, const FHitResult& Hit) {

}

void AEnemy_Base::OnSeePawn(APawn* OtherPawn) {

}

void AEnemy_Base::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {

}

void AEnemy_Base::OnActorOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

}
