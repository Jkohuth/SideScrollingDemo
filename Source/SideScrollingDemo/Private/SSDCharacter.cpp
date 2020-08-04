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
#include "DrawDebugHelpers.h"
#include "Updraft.h"
#include "Engine.h"
// Definition of tech debt
#include "EnemyCharacter.h"

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

	// CameraBounds would be better added in blueprints I think it's giving me errors with hot reload
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


}

// Called when the game starts or when spawned
void ASSDCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetCharacterState(ECharacterState::ACTIVE);
	if (CameraBounds) {
		CameraBounds->OnSSDCharacterBeginPlay(GetCapsuleComponent());
		CameraBounds->ResetCamera(this);
	}
	RespawnLocation = GetActorLocation();

	if (GetPlayerMovement()) {
		GetPlayerMovement()->SetMovementMode(MOVE_Walking);
	}
	else {
		UE_LOG(LogClass, Log, TEXT("PlayerMovement failed to load"));
	}
	//SetCharacterState(ECharacterState::ACTIVE);
	
}

// Called every frame
void ASSDCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AdjustFocusBarPercentage(DeltaTime);
	// Keep Player within Camera Bounds
	CameraBounds->UpdatePosition(GetCapsuleComponent(), DeltaTime);
}

// Called to bind functionality to input
void ASSDCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	//PlayerInputComponent->BindAction("Debug", IE_Pressed, this, &ASSDCharacter::DebugString);
	PlayerInputComponent->BindAction("Focus", IE_Pressed, this, &ASSDCharacter::TriggerFocus);
	PlayerInputComponent->BindAction("Focus", IE_Released, this, &ASSDCharacter::HaltFocus);
	PlayerInputComponent->BindAction("Swing", IE_Pressed, this, &ASSDCharacter::TryToSwing);
	//PlayerInputComponent->BindAction("BackDash", IE_Pressed, this, &ASSDCharacter::BackDash);
	//PlayerInputComponent->BindAction("Slide", IE_Pressed, this, &ASSDCharacter::OnStartSlide);
	//PlayerInputComponent->BindAction("Slide", IE_Released, this, &ASSDCharacter::OnStopSlide);
	//PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ASSDCharacter::Attack);
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
			//GetPlayerMovement()->TriggerGrindMovement(Rail->GetRailSpline(), SweepResult);
		}
	}
	else if (OtherActor && OtherActor->ActorHasTag(ECustomTags::RestTag)) {
		Health = FullHealth;
		FocusBarPercentage = 1.f;
		RespawnLocation = OtherActor->GetActorLocation();
	}
	else if (OtherActor && OtherActor->ActorHasTag(ECustomTags::BottomlessTag)) {
		Health = 0.f;
		DeathHandler();
	}
	else if (OtherActor && OtherActor->ActorHasTag(ECustomTags::WindTag)) {
		AUpdraft* updraft = Cast<AUpdraft>(OtherActor);
		if (updraft) {
			GetPlayerMovement()->TriggerUpdraftMovement(updraft);
		}
	}
	else if (OtherActor && OtherActor->ActorHasTag(ECustomTags::CaveTag)) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Cave Overlap");
		CameraBounds->SetCameraMode(ECameraMode::CAVE);

	}
	else if (OtherComp && OtherComp->ComponentHasTag(ECustomTags::LevelBoundsTag)) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Overlapped with level bounds");

	}


}
void ASSDCharacter::OnActorOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) { 
	if (OtherActor && OtherActor->ActorHasTag(ECustomTags::WindTag)) {
		GetPlayerMovement()->HaltUpdraftMovement();
	}
	else if (OtherActor && OtherActor->ActorHasTag(ECustomTags::CaveTag)) {
		CameraBounds->SetCameraMode(ECameraMode::MAIN);
	}
}
void ASSDCharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	// Check for Wall Movement

	if( OtherActor && OtherActor->ActorHasTag(ECustomTags::ClimbTag) && 
		FMath::IsNearlyEqual(FMath::Abs(Hit.ImpactNormal.Y), 1.f, 0.4f) 
 		&& GetPlayerMovement()->MovementMode == MOVE_Falling){ // Angle Tolerance
		if (GetPlayerMovement()) {
			GetPlayerMovement()->TriggerClimbMovement(Hit);
		}
	} 
	// Check for Rail Movement
	else if (OtherActor && OtherActor->ActorHasTag(ECustomTags::GrindTag)) {

		if (GetPlayerMovement()->CheckCustomMovementMode(ECustomMovementMode::MOVE_Grind)) return;

		if (Hit.ImpactNormal.Y > 0.8f || Hit.ImpactNormal.Z <= -0.1f) return;

		ARail* Rail = Cast<ARail>(OtherActor);
		if (Rail) {
			GetPlayerMovement()->TriggerGrindMovement(Rail->GetRailSpline(), Hit);
		}
	}
	else if (OtherComp && OtherComp->ComponentHasTag(ECustomTags::EnvironmentDamageTag)) {
		ReceiveEnvironmentalDamage(Hit);
	}
}
void ASSDCharacter::InitializeLevelBounds(UPrimitiveComponent* Bounds) {
	if (CameraBounds && Bounds) {
		CameraBounds->SetLevelBounds(Bounds);
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
void ASSDCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) {
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	
	if (!GetPlayerMovement()) return;

	switch (PrevMovementMode) {
	case EMovementMode::MOVE_Walking:
		CameraBounds->setLockCameraToBottom(false);
	}
	switch (GetPlayerMovement()->MovementMode) {
	case EMovementMode::MOVE_Walking:
		CameraBounds->setLockCameraToBottom(true);
		break;
	}
}
void ASSDCharacter::SetCharacterState(ECharacterState NewCharacterState){
	CharacterState = NewCharacterState;
}
ECharacterState ASSDCharacter::GetCharacterState(){
	return CharacterState;
}


// This will be removed when I use animations to send Anim notifies
void ASSDCharacter::InflictDamageHandler(bool isHit, TArray<FHitResult> HitArray) {
	if (isHit) {
		for (FHitResult Hit : HitArray) {
			if (Hit.GetActor() && Hit.GetActor()->ActorHasTag(ECustomTags::EnemyTag)) {
				// I know I need to make an enemy parent class but I don't feel like refactoring shit right now so this is tech debt
				InflictDamage(Hit.GetActor());
			}
		}
	}
}
// DAMAGE

void ASSDCharacter::InflictDamage(AActor* ImpactActor){
	AController* controller = Cast<AController>(GetController());
	if (controller != nullptr) {
		if ((ImpactActor != nullptr) && (ImpactActor != this)) {
			TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
			FDamageEvent DamageEvent(ValidDamageTypeClass);

			const float DamageAmount = 1.0f;
			ImpactActor->TakeDamage(DamageAmount, DamageEvent, controller, this);
	
		}
	}
}

float ASSDCharacter::ReceiveDamage_Implementation(float Damage, struct FPointDamageEvent const & PointDamageEvent, class AController *EventInstigator, AActor *DamageCauser){
	FDamageEvent DamageEvent = PointDamageEvent;
	//const float ActualDamage = t
	const float ActualDamage = this->TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	//Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "ReceiveDamage Called");

	// This ensures the player won't get hit repeatedly by the same creature

	if (CanReceiveDamage()) {
		GetPlayerMovement()->KnockBack(PointDamageEvent.HitInfo);

		if (ActualDamage > 0.f) {
			Health -= ActualDamage;
			DeathHandler();
		}
		immuneToDamage = true;
		GetWorld()->GetTimerManager().SetTimer(DamageImmunityTimer, this, &ASSDCharacter::ResetDamageImmunity, immunityDurationInSeconds, false);
	}
	return ActualDamage;
}
void ASSDCharacter::ReceiveEnvironmentalDamage(FHitResult HitInfo) {
	if (CanReceiveDamage()) {
		GetPlayerMovement()->KnockBack(HitInfo);
		Health -= EnvironmentDamage;
		DeathHandler();
	}
}
void ASSDCharacter::DeathHandler() {
	if (Health <= 0.f && GetCharacterState() != ECharacterState::DEAD) {
		TriggerDeathAnim();
		SetCharacterState(ECharacterState::DEAD);
	}
}

void ASSDCharacter::ResetDamageImmunity() {
	immuneToDamage = false;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Damage is reset");

}
bool ASSDCharacter::CanReceiveDamage() {
	if (CharacterState == ECharacterState::ACTIVE && !immuneToDamage) {
		return true;
	}
	return false;
}
void ASSDCharacter::Respawn(FVector LastCheckPoint){
	Health = FullHealth;
	SetCharacterState(ECharacterState::ACTIVE);
	FocusBarPercentage = 1.f;
	SetActorLocation(LastCheckPoint); // May need to use a different method
	CameraBounds->ResetCamera(this);
}
bool ASSDCharacter::IsDead(){
	if(GetCharacterState() == ECharacterState::DEAD)
		return true;
	else {
		return false;
	}
}

// Focus
bool ASSDCharacter::IsFocused() const {
	return bFocused;
}
void ASSDCharacter::TriggerFocus() {
	bFocused = true;
	GetPlayerMovement()->TriggerFocusMovement();
	TriggerFocus_BP();
}
void ASSDCharacter::HaltFocus() {
	bFocused = false;
	GetPlayerMovement()->HaltFocusMovement();
	HaltFocus_BP();
}
void ASSDCharacter::AdjustFocusBarPercentage(float DeltaTime) {
	if (!bFocused) return;
	if (FocusBarPercentage > 0.f) {
		FocusBarPercentage -= DeltaTime / FocusReductionRate;
	}
	else
	{
		if (GetCharacterState() != ECharacterState::DEAD) {
			float DamageAmount = 1.f;
			FHitResult Hit(1.f);
			TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
			FPointDamageEvent DamageEvent(DamageAmount, Hit, GetActorForwardVector(), ValidDamageTypeClass);
			ReceiveDamage(1.f, DamageEvent, GetController(), this);
			if (Health > .0f) {
				FocusBarPercentage = 1.f;
			}
		}
		else if (GetCharacterState() == ECharacterState::DEAD) {
			HaltFocus();
		}
	}
	
}

// Swing
void ASSDCharacter::TryToSwing() {
	FVector StartTrace = GetActorLocation();
	StartTrace.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector EndTrace = StartTrace;
	EndTrace.Z += 1.f; // I don't think you can do shape traces in place


	FVector HookSize = FVector(30.f, 70.f, 70.f); // Welcome to arbitary land
	FCollisionShape HookVolume = FCollisionShape::MakeBox(HookSize);
	
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	FHitResult Hit;
	GetWorld()->SweepSingleByChannel(Hit, StartTrace, EndTrace, FQuat::Identity, ECollisionChannel::ECC_WorldStatic, HookVolume);
	DrawDebugBox(GetWorld(), StartTrace, HookSize, FColor::Red, false, 2.f);

	if (Hit.GetActor() && Hit.GetActor()->ActorHasTag(ECustomTags::SwingTag)) {
		GetPlayerMovement()->TriggerSwingMovement(Hit.GetActor()->GetActorLocation());
	}
}
bool ASSDCharacter::IsSwinging() const {
	return GetPlayerMovement() && GetPlayerMovement()->IsSwinging();
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