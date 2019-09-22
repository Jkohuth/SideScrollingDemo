// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyPawn.h"
#include "Components/BoxComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "CustomFloatingPawnMovement.h"
#include "Engine.h"
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

	PawnSensor = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Sensor"));
	PawnSensor->SensingInterval = .25f; // How often does pawn react
	PawnSensor->bOnlySensePlayers = false;
	PawnSensor->SetPeripheralVisionAngle(35.f);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnemyMesh"));
	MeshComponent->SetupAttachment(RootComponent);
	//MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PawnMovement = CreateDefaultSubobject<UCustomFloatingPawnMovement>(TEXT("CustomMovement"));
	PawnMovement->UpdatedComponent = RootComponent;

}
void AEnemyPawn::PostInitializeComponents() {
	Super::PostInitializeComponents();

	PawnSensor->OnSeePawn.AddDynamic(this, &AEnemyPawn::OnSeePawn);
	PawnSensor->OnHearNoise.AddDynamic(this, &AEnemyPawn::OnHearNoise);
	
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
void AEnemyPawn::OnHearNoise(APawn *OtherActor, const FVector &Location, float Volume) {
	//const FString VolumeDesc = FString::Printf(TEXT(" at volume %f"), Volume);
	//FString message = TEXT("Heard Actor ") + OtherActor->GetName() + VolumeDesc;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, message);
}
void AEnemyPawn::OnSeePawn(APawn* OtherPawn) {
	//FString message = TEXT("Saw Actor ") + OtherPawn->GetName();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, message);
}

float AEnemyPawn::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) {
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f) {
		Health -= ActualDamage;
		if (Health <= 0.f) {
			SetLifeSpan(0.001f);
		}
	}
	return ActualDamage;
}
void AEnemyPawn::InflictDamage(AActor* ImpactActor, const FHitResult& Hit) {
	AController* EnemyController = Cast<AController>(GetController());
	if (EnemyController != nullptr && (ImpactActor != nullptr) && (ImpactActor != this)) {
		TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
		//FDamageEvent DamageEvent(ValidDamageTypeClass);
		const float DamageAmount = 1.0f;

		FPointDamageEvent DamageEvent(DamageAmount, Hit, GetActorForwardVector(), ValidDamageTypeClass);

		//AMalePlayer* player = Cast<AMalePlayer>(ImpactActor);
		//if (player)
			//player->TakeDamage(DamageAmount, DamageEvent, EnemyController, this);

	}
}
/*
void AEnemyPawn::WasAttacked() {
	this->DisableActor();
}
void AEnemyPawn::DisableActor() {
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

}*/