// Fill out your copyright notice in the Description page of Project Settings.


#include "SSDCharacter.h"
#include "SSDPlayerMovementComponent"

// Sets default values
ASSDCharacter::ASSDCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USSDPlayerMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	RootComponent = GetCapsuleComponent();

	GetCapsuleComponent()->InitCapsuleSize(24.5, 94.5);
	GetCapsuleComponent()->SetWorldRotation(FRotator(0.0f, 0.0f, 90.f));

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ASSDCharacter::OnHit);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ASSDCharacter::OnActorOverlapBegin);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ASSDCharacter::OnActorOverlapEnd);

	CameraBounds = CreateDefaultSubobject<UCameraBoundingBoxComponent>(TEXT("CameraBounds"));
	CameraBounds->GetCameraComponent()->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));


	PlayerMovement = Cast<USSDPlayerMovementComponent>(GetCharacterMovement());

	if (PlayerMovement) {
		PlayerMovement->UpdatedComponent = RootComponent;
	}

	AutoPossessPlayer = EAutoReceiveInput::Player0;

}

// Called when the game starts or when spawned
void ASSDCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetCharacterState(ECharacterState::ACTIVE);
	
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
	PlayerInputComponent->BindAction("Focus", IE_Pressed, this, &ASSDCharacter::ActivateFocus);
	PlayerInputComponent->BindAction("Focus", IE_Released, this, &ASSDCharacter::DeactivateFocus);
	PlayerInputComponent->BindAction("BackDash", IE_Pressed, this, &ASSDCharacter::BackDash);
	PlayerInputComponent->BindAction("Slide", IE_Pressed, this, &ASSDCharacter::OnStartSlide);
	PlayerInputComponent->BindAction("Slide", IE_Released, this, &ASSDCharacter::OnStopSlide);

	PlayerInputComponent->BindAxis("MoveRight", this, &ASSDCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &ASSDCharacter::MoveUp);

}

void ASSDCharacter::MoveRight(float Value) {
	if (PlayerMovement && (PlayerMovement->UpdatedComponent == RootComponent)) {
		GetPlayerMovement()->MoveRightInput(Value);
	}
}
void ASSDCharacter::MoveRight(float Value) {
	if (PlayerMovement && (PlayerMovement->UpdatedComponent == RootComponent)) {
		GetPlayerMovement()->MoveUpInput(Value);
	}
}

// EVENT TRIGGERS
void ASSDCharacter::OnActorOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { }
void ASSDCharacter::OnActorOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) { }
void ASSDCharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {

	// Check for Wall Movement
	if(OtherActor != nullptr && OtherActor->ActorHasTag(ECustomTags::WallTag) && FMath::IsNearlyEqual(FMath::Abs(Hit.ImpactNormal.Y), 1.f, 0.2f) 
		&& PlayerMovement->MovementMode == MOVE_Falling){ // Angle Tolerance
		if (GetPlayerMovement()) {
			GetPlayerMovement()->TriggerWallMovement(Hit);
		}
	} 
	// Check for Rail Movement
	else if (OtherActor && OtherActor->ActorHasTag(ECustomTags::RailTag)) {
		if (GetPlayerMovement()->CheckCustomMovementMode(ECustomMovementMode::MOVE_Rail)) return;

		ARail* Rail = Cast<ARail>(OtherActor);
		if (Rail) {
			GetPlayerMovement()->AttachToRail(Rail->GetRailSpline());
		}
	}
}

// JUMP
void ASSDCharacter::Jump() {

	GetPlayerMovement()->OnJumpInput();
	//SetupJumpCalculations();
	GetPlayerMovement()->isJumping = true;

	Super::Jump();
}
void ASSDCharacter::StopJumping() {
	Super::StopJumping();
	if (GetPlayerMovement()->MovementMode == MOVE_Falling) {
		GetPlayerMovement()->SetGravity(GetPlayerMovement()->FallingGravityScalar);
		GetPlayerMovement()->isJumping = false;
	}
}
void ASSDCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();
	GetPlayerMovement()->isJumping = false;
	// Think of a better way to do this

	// So here I would Like to clean up what I have. Setting Gravity is important but 
	// I may want to have a ton of different gravitys based on situation
	GetPlayerMovement()->SetGravity(GetPlayerMovement()->FallingGravityScalar);
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

void ASSDCharacter::TakeDamage_Implementation(float Damage, struct FPointDamageEvent const & PointDamageEvent, class AController *EventInstigator, AActor *DamageCauser){
	FDamageEvent DamageEvent = PointDamageEvent;
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	// This ensures the player won't get hit repeatedly by the same creature
	if (!immuneDamage && CharacterState == ECharacterState::ACTIVE) {
		GetPlayerMovement()->KnockBack(PointDamageEvent.HitInfo);

		if (ActualDamage > 0.f) {
			Health -= ActualDamage;
			if (Health <= 0.f && GetCharacterState() != ECharacterState::DEAD) {
				TriggerDeathAnim();
				SetCharacterState(ECharacterState::DEAD);
			}
		}
		immuneDamage = true;
	}

	return ActualDamage;
}
void ASSDCharacter::PostDamageImmunity(float DeltaTime){
	currentDaamgeFrame++;
	if (currentDamageFrame >= frameImmunity) {
		currentDamageFrame = 0.f;
		immuneDamage = false;
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