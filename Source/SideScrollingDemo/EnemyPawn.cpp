// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyPawn.h"
#include "Components/CapsuleComponent.h"
//#include "Perception/PawnSensingComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "MalePlayer.h"
#include "Engine.h"
#include "Constants.h"
// Sets default values
AEnemyPawn::AEnemyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Box"));
	//BoxComponent->InitBoxExtent(FVector(10.0f, 40.0f, 40.0f));

	RootComponent = CapsuleComponent;

	CapsuleComponent->OnComponentHit.AddDynamic(this, &AEnemyPawn::OnHit);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));

	//PawnSensor = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Sensor"));
	//PawnSensor->SensingInterval = .25f; // How often does pawn react
	//PawnSensor->bOnlySensePlayers = false;
	//PawnSensor->SetPeripheralVisionAngle(35.f);
	
	

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnemyMesh"));
	MeshComponent->SetupAttachment(RootComponent);
	//MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PawnMovement = CreateDefaultSubobject<UPawnMovementComponent>(TEXT("PawnMovement"));
	if(PawnMovement){
		PawnMovement->UpdatedComponent = RootComponent;
	}
}
void AEnemyPawn::PostInitializeComponents() {
	Super::PostInitializeComponents();

	//PawnSensor->OnSeePawn.AddDynamic(this, &AEnemyPawn::OnSeePawn);
	//PawnSensor->OnHearNoise.AddDynamic(this, &AEnemyPawn::OnHearNoise);
	
	if (GetMovementComponent() && CapsuleComponent) {
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
	
	tickCounter++;
	if (FMath::IsNearlyZero(FMath::Fmod(tickCounter, secondDivsor))) {
		OnCustomSense(this, DeltaTime);
		tickCounter = 0.f;
	}

}

void AEnemyPawn::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {

}
/*void AEnemyPawn::OnHearNoise(APawn *OtherActor, const FVector &Location, float Volume) {
	//const FString VolumeDesc = FString::Printf(TEXT(" at volume %f"), Volume);
	//FString message = TEXT("Heard Actor ") + OtherActor->GetName() + VolumeDesc;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, message);
}
void AEnemyPawn::OnSeePawn(APawn* OtherPawn) {
	// Handle all the chain of events logic here
	Attack(OtherPawn); 
}*/
void AEnemyPawn::Attack(APawn* OtherPawn){
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Attack was called from enemy pawn");

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
void AEnemyPawn::OnCustomSense(APawn* OtherPawn, float DeltaTime) {
	
	float halfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	float radius = GetCapsuleComponent()->GetUnscaledCapsuleRadius();


	FVector Start = GetActorLocation();
	//FVector End = Start + (focusRadius *FVector(1.f)); // Something is a miss here, I want to draw a single sphere in place
	FVector End = Start;
	End.Z += halfHeight;
	FCollisionQueryParams CollisionParams(FName(TEXT("Sight")), true, this);
	FCollisionShape capsule = FCollisionShape::MakeCapsule(radius, halfHeight);

	CollisionParams.AddIgnoredActor(this);

	//CollisionParams.bTraceComplex = true;
	bool isHit = GetWorld()->SweepMultiByChannel(ObjsInSight, Start, End, FQuat::Identity, ECollisionChannel::ECC_WorldDynamic, capsule, CollisionParams);

	if (isHit) {
		 
		//UE_LOG(LogClass, Log, TEXT("Size of FocusSphereZone %s"), *FString::FromInt(FocusSphereZone.Num()));

		for (auto& Hit : ObjsInSight) {
			if (Hit.GetActor()) {
				AMalePlayer* player = Cast<AMalePlayer>(Hit.GetActor());
				if (player) {
					GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, "Hit the other player");
					//Start.Z += 40.f;
					//this->SetActorLocation(Start);
					//FHitResult Hit;
					//FVector DeltaMovement = FVector(0.f, 0.f, 40.f)*DeltaTime;
					//PawnMovement->SafeMoveUpdatedComponent(DeltaMovement, FQuat::Identity, true, Hit);
				}

				//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, *tmp);
			}
		}
	}
	ObjsInSight.Empty();
	//DrawDebugSphere(GetWorld(), Start, focusRadius, 10, FColor::Red, false, 2.0f);
	DrawDebugCapsule(GetWorld(), End, halfHeight, radius, FQuat::Identity, FColor::Red);
}