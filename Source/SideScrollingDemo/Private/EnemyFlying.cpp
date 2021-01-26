// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFlying.h"
#include "SSDCharacter.h"
#include "Engine.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/ArrowComponent.h"

// Sets default values
AEnemyFlying::AEnemyFlying()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyFlying::BeginPlay()
{
	Super::BeginPlay();
	if (GetMovementComponent() != nullptr) {
		GetMovementComponent()->SetPlaneConstraintAxisSetting(LockXAxis);
	}

	// Check twice a second to see if the bird is rendered to the screen
	GetWorld()->GetTimerManager().SetTimer(isRenderedTimer, this, &AEnemyFlying::TriggerScreenRenderedCheck, 1, true, 0.5);

	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AEnemyFlying::OnHit);
	GetCharacterMovement()->SetPlaneConstraintAxisSetting(LockXAxis);
	GetCharacterMovement()->bConstrainToPlane = true;
}

// Called every frame
void AEnemyFlying::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetCharacterMovement()->MovementMode != MOVE_Flying) GetCharacterMovement()->SetMovementMode(MOVE_Flying); 

}

float AEnemyFlying::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) {
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f) {
		Health -= ActualDamage;
		if (Health <= 0.f) {
			SetLifeSpan(0.001f);
		}
	}
	return ActualDamage;
}
void AEnemyFlying::InflictDamage(AActor* ImpactActor, const FHitResult& Hit) {
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

void AEnemyFlying::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	if (OtherActor && OtherActor->ActorHasTag(ECustomTags::PlayerTag)) {
		InflictDamage(OtherActor, Hit);
	}

}

bool AEnemyFlying::isRenderedToScreen(APlayerController *playerController) {

	// Change the Controller Name, if controller isn't functioning then treat it as nevered rendered
	if (playerController == nullptr || !playerController->IsValidLowLevel()) {
		return false;
	}
	FVector2D ScreenLocation;
	playerController->ProjectWorldLocationToScreen(GetActorLocation(), ScreenLocation);

	int32 ScreenWidth = 0;
	int32 ScreenHeight = 0;
	playerController->GetViewportSize(ScreenWidth, ScreenHeight);

	return ScreenLocation.X >= 0 && ScreenLocation.Y >= 0 && ScreenLocation.X < ScreenWidth && ScreenLocation.Y < ScreenHeight;

}
