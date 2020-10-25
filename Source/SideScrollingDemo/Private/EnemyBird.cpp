// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBird.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "SSDCharacter.h"
#include "Engine.h"
// Sets default values
AEnemyBird::AEnemyBird()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SphereComponent->SetSphereRadius(100.f);
	RootComponent = SphereComponent;

	GetSphereComponent()->OnComponentHit.AddDynamic(this, &AEnemyBird::OnHit);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(SphereComponent);

	/*PawnSensor = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Sensor"));
	PawnSensor->SensingInterval = .25f; // How often does pawn react
	PawnSensor->bOnlySensePlayers = false;
	PawnSensor->SetPeripheralVisionAngle(35.f);
	PawnSensor->HearingThreshold = 600.f;
	PawnSensor->LOSHearingThreshold = 600.f;*/

}

// Called when the game starts or when spawned
void AEnemyBird::BeginPlay()
{
	Super::BeginPlay();
	if (GetMovementComponent() != nullptr) {
		GetMovementComponent()->SetPlaneConstraintAxisSetting(LockXAxis);
	}

	// Check twice a second to see if the bird is rendered to the screen
	GetWorld()->GetTimerManager().SetTimer(isRenderedTimer, this, &AEnemyBird::TriggerScreenRenderedCheck, 1, true, 0.5);

}

// Called every frame
void AEnemyBird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemyBird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
float AEnemyBird::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) {
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "Get Hit Got Wrecked");
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f) {
		Health -= ActualDamage;
		if (Health <= 0.f) {
			SetLifeSpan(0.001f);
		}
	}
	return ActualDamage;
}
void AEnemyBird::InflictDamage(AActor* ImpactActor, const FHitResult& Hit) {
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
	}
}

void AEnemyBird::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	if (OtherActor && OtherActor->ActorHasTag(ECustomTags::PlayerTag)) {
		InflictDamage(OtherActor, Hit);
	}

}

bool AEnemyBird::isRenderedToScreen(APlayerController *playerController) {

	// Change the Controller Name, if controller isn't functioning then treat it as nevered rendered
	if (playerController == nullptr || !playerController->IsValidLowLevel()) {
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Controller is broke yo");
		return false;
	}
	FVector2D ScreenLocation;
	playerController->ProjectWorldLocationToScreen(GetActorLocation(), ScreenLocation);

	int32 ScreenWidth = 0;
	int32 ScreenHeight = 0;
	playerController->GetViewportSize(ScreenWidth, ScreenHeight);
	
	return ScreenLocation.X >= 0 && ScreenLocation.Y >= 0 && ScreenLocation.X < ScreenWidth && ScreenLocation.Y < ScreenHeight;

}
