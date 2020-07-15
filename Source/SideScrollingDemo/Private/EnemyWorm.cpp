// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyWorm.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "SSDCharacter.h"

// Sets default values
AEnemyWorm::AEnemyWorm()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	RootComponent = GetCapsuleComponent();
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AEnemyWorm::OnActorOverlapBegin);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	//ArrowComponent->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	ArrowComponent->SetupAttachment(RootComponent);
	ArrowComponent->SetRelativeLocation(FVector::ZeroVector);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->OnComponentHit.AddDynamic(this, &AEnemyWorm::OnHit);

	//SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	//SplineComponent->SetupAttachment(RootComponent);
	//SplineComponent->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	BoxComponent->SetupAttachment(RootComponent);
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyWorm::OnActorOverlapBegin);
}

// Called when the game starts or when spawned
void AEnemyWorm::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyWorm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyWorm::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEnemyWorm::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) {
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f) {
		Health -= ActualDamage;
		if (Health <= 0.f) {
			SetLifeSpan(0.001f);
		}
	}
	return ActualDamage;
}
void AEnemyWorm::InflictDamage(AActor* ImpactActor, const FHitResult& Hit) {
	AController* EnemyController = Cast<AController>(GetController());
	if (EnemyController != nullptr && (ImpactActor != nullptr) && (ImpactActor != this)) {
		TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
		//FDamageEvent DamageEvent(ValidDamageTypeClass);
		const float DamageAmount = 1.0f;

		FPointDamageEvent DamageEvent(DamageAmount, Hit, GetActorForwardVector(), ValidDamageTypeClass);

		ASSDCharacter* player = Cast<ASSDCharacter>(ImpactActor);
		if (player) {
			player->ReceiveDamage(DamageAmount, DamageEvent, EnemyController, this);
		}
	}/*	*/
}
void AEnemyWorm::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Enemy Pawn on Hit was called");
	if (OtherActor != nullptr && OtherActor->ActorHasTag(ECustomTags::PlayerTag)) {
		InflictDamage(OtherActor, Hit);
	}
}
void AEnemyWorm::OnActorOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor != nullptr && OtherActor->ActorHasTag(ECustomTags::PlayerTag)) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "OverLapped with the player");

	}

}