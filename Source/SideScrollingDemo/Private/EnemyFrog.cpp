// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFrog.h"
#include "Perception/PawnSensingComponent.h"
#include "Engine.h"
#include "SSDCharacter.h"
#include "Perception/AIPerceptionComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogFrog, Log, All);

// Sets default values
AEnemyFrog::AEnemyFrog()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//GetCapsuleComponent()->SetCollisionObjectType(COLLISION_ENEMY);
	//GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_ENEMY, ECollisionResponse::ECR_Block);

	//PawnSensor = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Sensor"));
	//PawnSensor->SensingInterval = .25f; // How often does pawn react
	//PawnSensor->bOnlySensePlayers = false;
	//PawnSensor->SetPeripheralVisionAngle(35.f);

	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetCharacterMovement()->JumpZVelocity = 400.f;

	//AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AISense"));

}

// Called when the game starts or when spawned
void AEnemyFrog::BeginPlay()
{
	Super::BeginPlay();
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AEnemyFrog::OnHit);

	OriginPos = GetActorLocation();
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintAxisSetting(LockXAxis);
}

// Called every frame
void AEnemyFrog::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Expensive call but stick with me here
	/*if(!playerSpotted)
		AddMovementInput(GetActorForwardVector(), 10.f);
	//AddMovementInput(GetActorForwardVector());
	//float dist = FVector::Dist(GetActorLocation(), OriginPos);
	//UE_LOG(LogClass, Log, TEXT("Enemy Forward Vector %s"), *GetActorForwardVector().ToCompactString());
	//UE_LOG(LogClass, Log, TEXT("Distance %s"), *FString::SanitizeFloat(dist));
	if (!rotated && FVector::Dist(GetActorLocation(), OriginPos) > MaxWalkRadius) {
		TurnAround();
		rotated = true;
	}
	if (GetActorLocation().Equals(OriginPos, 20.f)) {
		rotated = false; // Crude Fix
	}*/
}

// Called to bind functionality to input
void AEnemyFrog::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void AEnemyFrog::PostInitializeComponents() {
	Super::PostInitializeComponents();
	//PawnSensor->OnSeePawn.AddDynamic(this, &AEnemyFrog::OnSeePawn);
	//PawnSensor->OnHearNoise.AddDynamic(this, &AEnemyFrog::OnHearNoise);
}

void AEnemyFrog::TurnAround() {
	FQuat EnemyDirection;
	if (FMath::IsNearlyEqual(GetActorForwardVector().Y, 1.0f, .1f)) {
		EnemyDirection = FQuat::MakeFromEuler(FVector::UpVector * -90.f);
		//UE_LOG(LogEnemy, Log, TEXT("UpVector %s"), *FVector::UpVector.ToString());

	}
	else if (FMath::IsNearlyEqual(GetActorForwardVector().Y, -1.0f, .1f)) {
		EnemyDirection = FQuat::MakeFromEuler(FVector::UpVector * 90.f);
	}
	SetActorRotation(EnemyDirection);

}

void AEnemyFrog::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpluse, const FHitResult& Hit) {
	//UE_LOG(LogEnemy, Log, TEXT("Enemy Hit %s"), *OtherActor->GetName());
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "OnHit was called");

	TurnAround();
	if (OtherActor && OtherActor->ActorHasTag(ECustomTags::PlayerTag)) {
		InflictDamage(OtherActor, Hit);
	}


}
float AEnemyFrog::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) {
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f) {
		Health -= ActualDamage;
		if (Health <= 0.f) {
			SetLifeSpan(0.001f);
		}
	}
	return ActualDamage;
}
void AEnemyFrog::InflictDamage(AActor* ImpactActor, const FHitResult& Hit) {
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

void AEnemyFrog::TriggerAttack() {
	GetWorld()->GetTimerManager().SetTimer(AnticipationTimer, this, &AEnemyFrog::DoAttack, 1, false, 0.5);
	playerSpotted = true;
	PlayerSensed();
}
void AEnemyFrog::DoAttack() {
	GetWorld()->GetTimerManager().ClearTimer(AnticipationTimer);
	playerSpotted = false;
	Jump();
	GetCharacterMovement()->Velocity.Y += GetActorForwardVector().Y * lungeVelocity;
}