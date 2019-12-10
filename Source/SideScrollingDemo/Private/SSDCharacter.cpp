// Fill out your copyright notice in the Description page of Project Settings.


#include "SSDCharacter.h"
#include "SSDPlayerMovementComponent.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CameraBoundingBoxComponent.h"
#include "Rail.h"
#include "Components/SplineComponent.h"
#include "Components/PostProcessComponent.h"
#include "GameFramework/PlayerController.h"

// Sets default values
ASSDCharacter::ASSDCharacter(const FObjectInitializer& ObjectInitializer)
	: ACharacter(ObjectInitializer.SetDefaultSubobjectClass<USSDPlayerMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;


	GetCapsuleComponent()->InitCapsuleSize(24.5, 94.5);
	//GetCapsuleComponent()->SetWorldRotation(FRotator(0.0f, 0.0f, 90.f));

	RootComponent = GetCapsuleComponent();

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ASSDCharacter::OnHit);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ASSDCharacter::OnActorOverlapBegin);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ASSDCharacter::OnActorOverlapEnd);

	CameraBounds = CreateDefaultSubobject<UCameraBoundingBoxComponent>(TEXT("CameraBounds"));
	//CameraBounds->GetCameraComponent()->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));

	CharacterEffects = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
	CharacterEffects->SetupAttachment(RootComponent);
	// Don't change brightness when `ing into the shade
	CharacterEffects->Settings.AutoExposureMinBrightness = 1.f;
	CharacterEffects->Settings.AutoExposureMaxBrightness = 1.f;
	CharacterEffects->bEnabled = false;

	PlayerMovement = Cast<USSDPlayerMovementComponent>(GetCharacterMovement());

	if (PlayerMovement) {
		PlayerMovement->UpdatedComponent = RootComponent;
	}
	else {
		UE_LOG(LogClass, Log, TEXT("PlayerMovement failed to load"));
	}

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	SetCharacterState(ECharacterState::ACTIVE);

}
void ASSDCharacter::OnConstruction() {
}
// Called when the game starts or when spawned
void ASSDCharacter::BeginPlay()
{
	Super::BeginPlay();
	CameraBounds->OnConstructionComponent();

	if (GetPlayerMovement()) {
		GetPlayerMovement()->SetMovementMode(MOVE_Walking);
	}
	else {
		UE_LOG(LogClass, Log, TEXT("PlayerMovement failed to load"));
	}
	CameraBounds->ResetCamera(this);
	//SetCharacterState(ECharacterState::ACTIVE);
	
}

// Called every frame
void ASSDCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Keep Player within Camera Bounds
	CameraBounds->UpdatePosition(GetCapsuleComponent());
}

// Called to bind functionality to input
void ASSDCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	//PlayerInputComponent->BindAction("Debug", IE_Pressed, this, &ASSDCharacter::DebugString);
	//PlayerInputComponent->BindAction("Focus", IE_Pressed, this, &ASSDCharacter::ActivateFocus);
	//PlayerInputComponent->BindAction("Focus", IE_Released, this, &ASSDCharacter::DeactivateFocus);
	//PlayerInputComponent->BindAction("BackDash", IE_Pressed, this, &ASSDCharacter::BackDash);
	//PlayerInputComponent->BindAction("Slide", IE_Pressed, this, &ASSDCharacter::OnStartSlide);
	//PlayerInputComponent->BindAction("Slide", IE_Released, this, &ASSDCharacter::OnStopSlide);

	PlayerInputComponent->BindAxis("MoveRight", this, &ASSDCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &ASSDCharacter::MoveUp);

}

void ASSDCharacter::MoveRight(float Value) {
	if (GetPlayerMovement() && (GetPlayerMovement()->UpdatedComponent == RootComponent)) {
		GetPlayerMovement()->MoveRightInput(Value);
	}
}
void ASSDCharacter::MoveUp(float Value) {
	if (GetPlayerMovement() && (GetPlayerMovement()->UpdatedComponent == RootComponent)) {
		GetPlayerMovement()->MoveUpInput(Value);
	}
}

// EVENT TRIGGERS
void ASSDCharacter::OnActorOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult){

	if (OtherActor && OtherActor->ActorHasTag(ECustomTags::GrindTag)) {
		ARail* Rail = Cast<ARail>(OtherActor);
		if (Rail) {
			GetPlayerMovement()->TriggerGrindMovement(Rail->GetRailSpline(), SweepResult);
		}
	}


}
void ASSDCharacter::OnActorOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) { }
void ASSDCharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	FString tmp = "hit something";
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, tmp);
	// Check for Wall Movement
	if(OtherActor && OtherActor->ActorHasTag(ECustomTags::ClimbTag) && FMath::IsNearlyEqual(FMath::Abs(Hit.ImpactNormal.Y), 1.f, 0.2f) 
		&& GetPlayerMovement()->MovementMode == MOVE_Falling){ // Angle Tolerance
		if (GetPlayerMovement()) {
			GetPlayerMovement()->TriggerClimbMovement(Hit);
		}
	} 
	// Check for Rail Movement
	else if (OtherActor && OtherActor->ActorHasTag(ECustomTags::GrindTag)) {
		if (GetPlayerMovement()->CheckCustomMovementMode(ECustomMovementMode::MOVE_Grind)) return;

		if (Hit.ImpactNormal.Y > 0.8f) return;

		ARail* Rail = Cast<ARail>(OtherActor);
		if (Rail) {
			GetPlayerMovement()->TriggerGrindMovement(Rail->GetRailSpline(), Hit);
		}
	}
}

// JUMP
void ASSDCharacter::Jump() {
	if (GetPlayerMovement()) {
		GetPlayerMovement()->OnJumpInput();
		GetPlayerMovement()->isJumping = true;
	}
	
	Super::Jump();
}
void ASSDCharacter::StopJumping() {
	Super::StopJumping();
	if (GetPlayerMovement() && GetPlayerMovement()->MovementMode == MOVE_Falling) {
		GetPlayerMovement()->SetCharacterGravity(GetPlayerMovement()->FallingGravityScalar); 
		GetPlayerMovement()->isJumping = false;
	}
}
void ASSDCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();
	if (GetPlayerMovement()) {
		GetPlayerMovement()->isJumping = false;
		GetPlayerMovement()->SetCharacterGravity(GetPlayerMovement()->FallingGravityScalar);

	}
	// Think of a better way to do this

	// So here I would Like to clean up what I have. Setting Gravity is important but 
	// I may want to have a ton of different gravitys based on situation

	//JumpActual();
}

// CHARACTER STATE

void ASSDCharacter::SetCharacterState(ECharacterState NewCharacterState){
	CharacterState = NewCharacterState;
}
ECharacterState ASSDCharacter::GetCharacterState(){
	return CharacterState;
}


// DAMAGE

void ASSDCharacter::InflictDamage(AActor* ImpactActor){
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController != nullptr) {
		if ((ImpactActor != nullptr) && (ImpactActor != this)) {
			TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
			FDamageEvent DamageEvent(ValidDamageTypeClass);

			const float DamageAmount = 1.0f;
			ImpactActor->TakeDamage(DamageAmount, DamageEvent, PlayerController, this);
	
		}
	}
}

float ASSDCharacter::ReceiveDamage_Implementation(float Damage, struct FPointDamageEvent const & PointDamageEvent, class AController *EventInstigator, AActor *DamageCauser){
	FDamageEvent DamageEvent = PointDamageEvent;
	//const float ActualDamage = t
	const float ActualDamage = this->TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	//Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "ReceiveDamage Called");

	// This ensures the player won't get hit repeatedly by the same creature
	if (CharacterState == ECharacterState::ACTIVE) {
		GetPlayerMovement()->KnockBack(PointDamageEvent.HitInfo);

		if (ActualDamage > 0.f) {
			Health -= ActualDamage;
			if (Health <= 0.f && GetCharacterState() != ECharacterState::DEAD) {
				TriggerDeathAnim();
				SetCharacterState(ECharacterState::DEAD);
			}
		}
		//immuneToDamage = true;
	}

	return ActualDamage;
}
void ASSDCharacter::PostDamageImmunity(float DeltaTime){
	currentDamageFrame++;
	if (currentDamageFrame >= frameImmunity) {
		currentDamageFrame = 0.f;
		immuneToDamage = false;
	}
}
void ASSDCharacter::Respawn(FVector LastCheckPoint){
	Health = FullHealth;
	SetCharacterState(ECharacterState::ACTIVE);
	SetActorLocation(LastCheckPoint); // May need to use a different method
}


// SLIDE 

// This slide implementation isn't finished and is based off the 
// slide from Epic Games "PlatformerGame" I think it is worth 
// Looking into how they instantiate the effect to draw out what I want
bool ASSDCharacter::IsGrinding() const {
	return GetPlayerMovement() && GetPlayerMovement()->IsGrinding();
}
bool ASSDCharacter::IsClimbing() const {
	return GetPlayerMovement() && GetPlayerMovement()->IsClimbing();
}

bool ASSDCharacter::IsSliding() const {
	return GetPlayerMovement() && GetPlayerMovement()->IsSliding();
}
bool ASSDCharacter::WantsToSlide() const {
	return bPressedSlide;
}
void ASSDCharacter::OnStartSlide(){

}
void ASSDCharacter::OnStopSlide(){
	
}