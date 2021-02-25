// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyHawk.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "SSDCharacter.h"
#include "Engine.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/ArrowComponent.h"

// Sets default values
AEnemyHawk::AEnemyHawk()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCollisionComponent()->OnComponentHit.AddDynamic(this, &AEnemyHawk::OnHit);

	OscillateMovement = FVector(0.f, 400.f, 40.f);
#if WITH_EDITORONLY_DATA
	ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	if (ArrowComponent)
	{
		ArrowComponent->ArrowColor = FColor(150, 200, 255);
		ArrowComponent->bTreatAsASprite = true;
		//ArrowComponent->SpriteInfo.Category = ConstructorStatics.ID_Characters;
		//ArrowComponent->SpriteInfo.DisplayName = ConstructorStatics.NAME_Characters;
		ArrowComponent->SetupAttachment(GetCollisionComponent());
		ArrowComponent->bIsScreenSizeScaled = true;
	}
#endif // WITH_EDITORONLY_DATA

}

// Called when the game starts or when spawned
void AEnemyHawk::BeginPlay()
{
	Super::BeginPlay();
	if (GetMovementComponent() != nullptr) {
		GetMovementComponent()->SetPlaneConstraintAxisSetting(LockXAxis);
	}

	// Check twice a second to see if the bird is rendered to the screen
	GetWorld()->GetTimerManager().SetTimer(isRenderedTimer, this, &AEnemyHawk::TriggerScreenRenderedCheck, 1, true, 0.5);

}

// Called every frame
void AEnemyHawk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemyHawk::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
float AEnemyHawk::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) {
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f) {
		Health -= ActualDamage;
		if (Health <= 0.f) {
			SetLifeSpan(0.001f);
		}
	}
	return ActualDamage;
}
void AEnemyHawk::InflictDamage(AActor* ImpactActor, const FHitResult& Hit) {
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

void AEnemyHawk::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	if (OtherActor && OtherActor->ActorHasTag(ECustomTags::PlayerTag)) {
		InflictDamage(OtherActor, Hit);
	}
	//TODO: Turn around if it collides with something other than the actor
	/*else {
		// Turn around if it hit something other than the player
		GetCollisionComponent()->SetWorldRotation(FRotator::MakeFromEuler(FVector(0.f, 180.f, 0.f)));
	}*/

}

bool AEnemyHawk::isRenderedToScreen(APlayerController *playerController) {

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
