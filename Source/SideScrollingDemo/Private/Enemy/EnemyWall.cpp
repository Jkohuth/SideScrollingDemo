// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyWall.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Controller.h"
#include "SSDCharacter.h"

// Sets default values
AEnemyWall::AEnemyWall()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	RootComponent = GetCapsuleComponent();
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AEnemyWall::OnActorOverlapBegin);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->OnComponentHit.AddDynamic(this, &AEnemyWall::OnHit);

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SplineComponent->SetupAttachment(RootComponent);
	SplineComponent->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
}

// Called when the game starts or when spawned
void AEnemyWall::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


float AEnemyWall::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) {
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f) {
		Health -= ActualDamage;
		if (Health <= 0.f) {
			SetLifeSpan(0.001f);
		}
	}
	return ActualDamage;
}
void AEnemyWall::InflictDamage(AActor* ImpactActor, const FHitResult& Hit) {
	AController* EnemyController = Cast<AController>(GetController());
	if (EnemyController != nullptr && (ImpactActor != nullptr) && (ImpactActor != this)) {
		TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
		//FDamageEvent DamageEvent(ValidDamageTypeClass);
		const float DamageAmount = 1.0f;

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Calling from INSIDE the inflict damage");
		FPointDamageEvent DamageEvent(DamageAmount, Hit, GetActorForwardVector(), ValidDamageTypeClass);

		ASSDCharacter* player = Cast<ASSDCharacter>(ImpactActor);
		if (player) {
			player->TakeDamage(DamageAmount, DamageEvent, EnemyController, this);
		}
	}/*	*/
}

void AEnemyWall::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Enemy Pawn on Hit was called");
	if (OtherActor != nullptr && OtherActor->ActorHasTag(ECustomTags::PlayerTag)) {
		InflictDamage(OtherActor, Hit);
	}
}
void AEnemyWall::OnActorOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

}