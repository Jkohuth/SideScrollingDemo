// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyCharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "Engine.h"
#include "Constants.h"


DEFINE_LOG_CATEGORY_STATIC(LogEnemy, Log, All);


// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AEnemyCharacter::OnHit);

	PawnSensor = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Sensor"));
	PawnSensor->SensingInterval = .25f; // How often does pawn react
	PawnSensor->bOnlySensePlayers = false;
	PawnSensor->SetPeripheralVisionAngle(35.f);

	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetCharacterMovement()->JumpZVelocity = 400.f;


}
void AEnemyCharacter::PostInitializeComponents() {
	Super::PostInitializeComponents();
	PawnSensor->OnSeePawn.AddDynamic(this, &AEnemyCharacter::OnSeePawn);
	PawnSensor->OnHearNoise.AddDynamic(this, &AEnemyCharacter::OnHearNoise);

}
// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	OriginPos = GetActorLocation();
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintAxisSetting(LockXAxis);
	
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	}

}
void AEnemyCharacter::TurnAround() {
	FQuat EnemyDirection;
	if (FMath::IsNearlyEqual(GetActorForwardVector().Y, 1.0f, .1f)) {
		EnemyDirection = FQuat::MakeFromEuler(FVector::UpVector * -90.f);
		UE_LOG(LogEnemy, Log, TEXT("UpVector %s"), *FVector::UpVector.ToString());

	}
	else if (FMath::IsNearlyEqual(GetActorForwardVector().Y, -1.0f, .1f)) {
		EnemyDirection = FQuat::MakeFromEuler(FVector::UpVector * 90.f);
	}
	SetActorRotation(EnemyDirection);
	
}

void AEnemyCharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpluse, const FHitResult& Hit) {
	//UE_LOG(LogEnemy, Log, TEXT("Enemy Hit %s"), *OtherActor->GetName());
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "OnHit was called");
	TurnAround();


}
float AEnemyCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) {
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f) {
		Health -= ActualDamage;
		if (Health <= 0.f) {
			SetLifeSpan(0.001f);
		}
	}
	return ActualDamage;
}
void AEnemyCharacter::InflictDamage() {
	AController* EnemyController = Cast<AController>(GetController());
	if (EnemyController != nullptr) {
		UE_LOG(LogClass, Log, TEXT("There is a player enemy controller"));
		FHitResult TraceResult(ForceInit);
		FVector Start = GetActorLocation();
		Start.Y += GetActorForwardVector().Y * GetCapsuleComponent()->GetUnscaledCapsuleRadius();
		FVector End = Start;
		float AttackRange = 30.f; //Still have to account to the raidus of the sphere


		End.Y += GetActorForwardVector().Y * AttackRange;
		FCollisionQueryParams CollisionParams(FName("EnemyAttack"));
		CollisionParams.AddIgnoredActor(this);

		bool isHit = GetWorld()->LineTraceSingleByChannel(TraceResult, Start, End, ECollisionChannel::ECC_Pawn, CollisionParams);

		DrawDebugLine(GetWorld(), Start, End, FColor::Red, true, 1.f);
		if (isHit) {
			AActor* ImpactActor = TraceResult.GetActor();
			if ((ImpactActor != nullptr) && (ImpactActor != this)) {
				TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
				FDamageEvent DamageEvent(ValidDamageTypeClass);

				const float DamageAmount = 1.0f;
				ImpactActor->TakeDamage(DamageAmount, DamageEvent, EnemyController, this);
			}
			
		}

	}
	/*
		// Perform a trace @See LineTraceSingle  
		FHitResult TraceResult(ForceInit);
		TraceHitForward(PlayerController, TraceResult);

		// If the trace return an actor, inflict some damage to that actor  
		AActor* ImpactActor = TraceResult.GetActor();
		if ((ImpactActor != nullptr) && (ImpactActor != this))
		{
			// Create a damage event  
			TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
			FDamageEvent DamageEvent(ValidDamageTypeClass);

			const float DamageAmount = 60.0f;
			ImpactActor->TakeDamage(DamageAmount, DamageEvent, PlayerController, this);
		}
	*/
}

void AEnemyCharacter::OnHearNoise(APawn *OtherActor, const FVector &Location, float Volume) {
	//const FString VolumeDesc = FString::Printf(TEXT(" at volume %f"), Volume);
	//FString message = TEXT("Heard Actor ") + OtherActor->GetName() + VolumeDesc;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, message);
}
void AEnemyCharacter::OnSeePawn(APawn* OtherPawn) {
	//FString message = TEXT("Saw Actor ") + OtherPawn->GetName();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, message);
	Jump();
	GetCharacterMovement()->Velocity.Y += GetActorForwardVector().Y * lungeVelocity;
	InflictDamage();
}
// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

