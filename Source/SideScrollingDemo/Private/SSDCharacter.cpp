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
void ASSDCharacter::OnActorOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

}
void ASSDCharacter::OnActorOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

}
void ASSDCharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {

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
