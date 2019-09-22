// Fill out your copyright notice in the Description page of Project Settings.

#include "MalePlayer.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "MalePlayerMovementComponent.h"
#include "Constants.h"
#include "CameraBoundingBox.h"
#include "Rail.h"
#include "Components/SplineComponent.h"
#include "WalkingPath.h"
//#include "Enemy.h"
#include "Engine.h"



// Sets default values
AMalePlayer::AMalePlayer(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMalePlayerMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCapsuleComponent()->InitCapsuleSize(24.5, 94.5);
	GetCapsuleComponent()->SetWorldRotation(FRotator(0.0f, 0.0f, 90.f));
	
	RootComponent = GetCapsuleComponent();
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AMalePlayer::OnHit);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMalePlayer::OnActorOverlapBegin);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AMalePlayer::OnActorOverlapEnd);
	//BoundingBox = CreateDefaultSubobject<ACameraBoundingBox>(TEXT("BoundingBox"));
	//BoundingBox->GetCameraComponent()->SetFieldOfView(10.f);
	// Create a camera boom attached to the root (capsule)
	/*CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Rotation of the character should not affect rotation of boom
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->TargetArmLength = 1000.f;
	CameraBoom->SocketOffset = SocketOffset;
	CameraBoom->RelativeRotation = FRotator(0.f, 180.f, 0.f);
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritPitch = false;	CameraBoom->bInheritYaw = false;	CameraBoom->bInheritRoll = false;

	// Create a camera and attach to boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	SideViewCameraComponent->bUsePawnControlRotation = false; // We don't want the controller rotating the camera*/
	
	//Camera = CreateDefaultSubobject<ACameraBoundingBox>(TEXT("Camera"));


	MalePlayerMovement = Cast<UMalePlayerMovementComponent>(GetCharacterMovement());
	
	if (MalePlayerMovement) {
		MalePlayerMovement->UpdatedComponent = RootComponent;
	}
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Initialize some variables here
	//focusTimeCount = 0;
	//recoilTime = focusRealTimeDilation * focusDilation;
	focusTimeCorrection = FVector(1.f, 0.2f, 0.5f);
	increaseRadiusRate = focusRadiusApex / focusIncreaseRadiusTime;
	decreaseRadiusRate = (focusRadiusApex - GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) / (recoilTime - focusIncreaseRadiusTime);
	GetCapsuleComponent()->bHiddenInGame = false;

}

// Called when the game starts or when spawned
void AMalePlayer::BeginPlay()
{
	Super::BeginPlay();
	SetCharacterState(ECharacterState::ACTIVE);

	
}

// Called every frame
void AMalePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (isJumping) countJumpTime += DeltaTime;
	if (isFocusing) { 
		UpdateFocus(DeltaTime);
	}
	if (immuneDamage) {
		PostDamageImmunity(DeltaTime);
	}
	//if (CustomMovementComponent->CustomMovementMode == ECustomMovementMode::MOVE_Wall) CameraBoom->SocketOffset = FVector::ZeroVector;

}

// Called to bind functionality to input
void AMalePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent){
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Debug", IE_Pressed, this, &AMalePlayer::DebugString);
	PlayerInputComponent->BindAction("Focus", IE_Pressed, this, &AMalePlayer::ActivateFocus);
	PlayerInputComponent->BindAction("Focus", IE_Released, this, &AMalePlayer::DeactivateFocus);

	PlayerInputComponent->BindAxis("MoveRight", this, &AMalePlayer::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &AMalePlayer::MoveUp);

}
void AMalePlayer::MoveRight(float Value) {
	if (MalePlayerMovement && (MalePlayerMovement->UpdatedComponent == RootComponent)) {
		MalePlayerMovement->MoveRightInput(Value);
		
		if (Value != 0.0) {
			Direction = FVector(0.f, -Value, 0.f);// Negative because of the worlds orientation
		}
	}
}
void AMalePlayer::MoveUp(float Value) {
	if (MalePlayerMovement) {
		MalePlayerMovement->MoveUpInput(Value);
	}
}
void AMalePlayer::NotifyJumpApex()
{
	Super::NotifyJumpApex(); 
	GetMalePlayerMovement()->isJumping = false;
	// Think of a better way to do this
	MalePlayerMovement->SetGravity(MalePlayerMovement->FallingGravityScalar);
	//JumpActual();
}

void AMalePlayer::Jump() {

	MalePlayerMovement->OnJumpInput();
	//SetupJumpCalculations();
	GetMalePlayerMovement()->isJumping = true;

	Super::Jump();
}
void AMalePlayer::StopJumping() {
	Super::StopJumping();
	if (MalePlayerMovement->MovementMode == MOVE_Falling) {
		MalePlayerMovement->SetGravity(MalePlayerMovement->FallingGravityScalar);
		GetMalePlayerMovement()->isJumping = false;
	}
}

void AMalePlayer::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	
	if(OtherActor != nullptr && OtherActor->ActorHasTag(ECustomTags::WallTag) && FMath::IsNearlyEqual(FMath::Abs(Hit.ImpactNormal.Y), 1.f, 0.2f) 
		&& MalePlayerMovement->MovementMode == MOVE_Falling){ // Angle Tolerance
		UE_LOG(LogClass, Log, TEXT("just hit the wall %s"), *OtherActor->GetName());
		if (MalePlayerMovement) {
			MalePlayerMovement->TriggerWallMovement(Hit);
		}
	}
	else if (OtherActor && OtherActor->ActorHasTag(ECustomTags::RailTag)) {
		if (MalePlayerMovement->CheckCustomMovementMode(ECustomMovementMode::MOVE_Rail)) return;
		UE_LOG(LogClass, Log, TEXT("Hit the rail "));

		ARail* Rail = Cast<ARail>(OtherActor);
		if (Rail) {
			MalePlayerMovement->AttachToRail(Rail->GetRailSpline());
		}
	}
}
void AMalePlayer::OnActorOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	if (OtherComp && OtherComp->ComponentHasTag(ECustomTags::PathTag)) {
		UE_LOG(LogClass, Log, TEXT("It Makes it this far"));

		AWalkingPath* WalkingPath = Cast<AWalkingPath>(OtherActor);
		if (WalkingPath) {
			UE_LOG(LogClass, Log, TEXT("It can detect the walking path"));
			MalePlayerMovement->AttachToPath(WalkingPath->GetSplineComponent());
		}
	}
}
void AMalePlayer::OnActorOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherComp && OtherComp->ComponentHasTag(ECustomTags::PathTag)) {
		UE_LOG(LogClass, Log, TEXT("You are no longer overlapping with the laddy with the path tag"));
	}
}


void AMalePlayer::DebugString() {
	
}
void AMalePlayer::UpdateFocus(float DeltaTime) {
	float dist = FVector::Dist(focusInitialLocation, GetActorLocation());
	//UE_LOG(LogClass, Log, TEXT("distance %s"), *FString::SanitizeFloat(dist));

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Distance %s", *FString::SanitizeFloat(dist));
	if (focusRadius < dist) {
		DeactivateFocus();
		return;
	}

	focusTimeCount += DeltaTime;
	if (focusTimeCount >= recoilTime) {
		DeactivateFocus();
		return;
	}
	/*if (focusTimeCount <= focusIncreaseRadiusTime) {
		focusRadius += increaseRadiusRate * DeltaTime;
	}
	else if (focusIncreaseRadiusTime < focusTimeCount && focusTimeCount <= recoilTime) {
		focusRadius -= decreaseRadiusRate * DeltaTime;
	}
	else if (recoilTime < focusTimeCount) {
		//	UE_LOG(LogClass, Log, TEXT("Held it for too long"));
		DeactivateFocus();
	}*/

	//UE_LOG(LogClass, Log, TEXT("FocusRadius %s | IncreaseRadiusRate: %s | DecreaseRadiusRate %s"), 
	//	*FString::SanitizeFloat(focusDilation), *FString::SanitizeFloat(increaseRadiusRate), 
	//	*FString::SanitizeFloat(decreaseRadiusRate));
	FVector Start = focusInitialLocation;
	//FVector End = Start + (focusRadius *FVector(1.f)); // Something is a miss here, I want to draw a single sphere in place
	FVector End = Start;
	End.X += 1.f;
	FCollisionQueryParams CollisionParams(FName(TEXT("Focus")), true, this);
	FCollisionShape Sphere = FCollisionShape::MakeSphere(focusRadius);

	CollisionParams.AddIgnoredActor(this);

	//CollisionParams.bTraceComplex = true;
	bool isHit = GetWorld()->SweepMultiByChannel(FocusSphereZone, Start, End, FQuat::Identity, ECollisionChannel::ECC_WorldDynamic, Sphere, CollisionParams);

	if (isHit) {

		//UE_LOG(LogClass, Log, TEXT("Size of FocusSphereZone %s"), *FString::FromInt(FocusSphereZone.Num()));

		for (auto& Hit : FocusSphereZone) {
			if (Hit.GetActor()) {
				Hit.GetActor()->CustomTimeDilation = focusDilation;
				FString tmp = FString::FString("The Actor that was dialated ") + GetDebugName(Hit.GetActor());
				//UE_LOG(LogClass, Log, TEXT("The Actor that was dialated: %s"), *tmp);

				//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, *tmp);
			}
		}
	}
	DrawDebugSphere(GetWorld(), Start, focusRadius, 10, FColor::Red, false, 2.0f);
	//DrawDebugSphere(GetWorld(), End, focusRadius, 10, FColor::Red, false, 2.0f);
	//GetWorldSettings()->SetTimeDilation(0.1f);

	this->CustomTimeDilation = focusDilationPlayer;
}
void AMalePlayer::ActivateFocus() {

	// Generate a sphere with a material to display effect
	// Expands rapidly contracts slowly
	// Does players time also get Dilated?
		// Will I have to adjust all the Phys_Custom components?
		// Or will I have him slowed like the rest?
	//focusRadius = 0.f;
	focusInitialLocation = GetActorLocation();
	isFocusing = true;
	focusTimeCount = 0.f;
	//UE_LOG(LogClass, Log, TEXT("ActivateFocus set FocusInitLocation %s"), *focusInitialLocation.ToCompactString());

	//UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1f);
}
void AMalePlayer::DeactivateFocus() {
	isFocusing = false;

	for (auto& Hit : FocusSphereZone) {
		if (Hit.GetActor()) {
			Hit.GetActor()->CustomTimeDilation = normalTime;
			FString tmp = GetDebugName(Hit.GetActor());
			UE_LOG(LogClass, Log, TEXT("The Actor that was dialated: %s"), *tmp);
		}
	}

	this->CustomTimeDilation = normalTime;
	//GetWorldSettings()->SetTimeDilation(normalTime);
	FocusSphereZone.Empty();
	float focusScalar = 1 - focusTimeCount / recoilTime;
	
	if (FMath::IsNearlyZero(focusScalar, 0.3f)) {
		focusScalar = .5f;
	}
	focusScalar = FMath::Pow(focusScalar, -1.f);
	
	focusImpluse = MalePlayerMovement->Velocity * focusScalar;// Find some interesting way to calculate this
	FString tmp = MalePlayerMovement->Velocity.ToCompactString() + " is the velocity and this is the scalar: " + FString::SanitizeFloat(focusScalar);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, *tmp);

	MalePlayerMovement->AccumulateForce(focusImpluse);
	/*//UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
	GetWorldSettings()->SetTimeDilation(1.f);
	FString tmp = FString::SanitizeFloat(focusTimeCount);
	UE_LOG(LogClass, Log, TEXT("The amount of times Tick has been called: %s"), *tmp);
	
	float difFocus = recoilTime - focusTimeCount;
	focusImpluse = MalePlayerMovement->MaxMovementSpeeds;
	
	tmp = FString::SanitizeFloat(difFocus);
	UE_LOG(LogClass, Log, TEXT("The difFocus: %s"), *tmp);
	UE_LOG(LogClass, Log, TEXT("The focusTime Correction: %s"), *focusTimeCorrection.ToCompactString());
	focusImpluse.X = focusImpluse.X / focusTimeCorrection.X;
	focusImpluse.Y = focusImpluse.Y / focusTimeCorrection.Y;
	focusImpluse.Y *= FMath::Sign(MalePlayerMovement->Velocity.Y); // it has to be negative because of level orientation
	focusImpluse.Z = focusImpluse.Z / focusTimeCorrection.Z;
	focusImpluse.Z *= FMath::Sign(MalePlayerMovement->Velocity.Z);

	tmp = focusImpluse.ToCompactString();
	UE_LOG(LogClass, Log, TEXT("The new Velocity: %s"), *tmp);
	
	MalePlayerMovement->AccumulateForce(focusImpluse);
	focusTimeCount = 0;*/
}

void AMalePlayer::InflictDamage(AActor* ImpactActor) {
	UE_LOG(LogClass, Log, TEXT("Here is the damage"));
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
float AMalePlayer::TakeDamage_Implementation(float Damage, struct FPointDamageEvent const & PointDamageEvent, class AController *EventInstigator, AActor *DamageCauser) {
	FDamageEvent DamageEvent = PointDamageEvent;
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	// This ensures the player won't get hit repeatedly by the same creature
	if (!immuneDamage && CharacterState == ECharacterState::ACTIVE) {
		MalePlayerMovement->KnockBack(PointDamageEvent.HitInfo);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Player Took Damage");
		if (ActualDamage > 0.f) {
			Health -= ActualDamage;
			if (Health <= 0.f && !isDead) {
				TriggerDeathAnim();
				SetCharacterState(ECharacterState::DEAD);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "He's dead Jim");
			}
		}
		immuneDamage = true;
	}

	return ActualDamage;
}
void AMalePlayer::Respawn(FVector LastCheckPoint) {
	Health = FullHealth;
	SetCharacterState(ECharacterState::ACTIVE);
	SetActorLocation(LastCheckPoint);
	
}
void AMalePlayer::SetCharacterState(ECharacterState NewCharacterState) {
	CharacterState = NewCharacterState;
}
ECharacterState AMalePlayer::GetCharacterState() {
	return CharacterState;
}
void AMalePlayer::PostDamageImmunity(float DeltaTime) {
	currentDamageFrame++;
	if (currentDamageFrame >= frameImmunity) {
		currentDamageFrame = 0.f;
		immuneDamage = false;
	}
}

// This is how I determine which how hight the player jump
void AMalePlayer::JumpCalculated() {
	if (MalePlayerMovement) {
		FString JumpString;
		FString timeString;

		float t01, t12, t02;

		DeltaX1.Z = MalePlayerMovement->JumpZVelocity * JumpTime;
		DeltaX2.Z = FMath::Pow(MalePlayerMovement->JumpZVelocity, 2) / (2 * (MalePlayerMovement->RisingGravityScalar)*(980));

		t01 = JumpTime;
		t12 = MalePlayerMovement->JumpZVelocity / (MalePlayerMovement->RisingGravityScalar * 980);
		t02 = t01 + t12;

		UE_LOG(LogClass, Log, TEXT("-----------Calculated Jump------------"));
		// Jump With Constant Velocity
		JumpString = FString::SanitizeFloat(DeltaX1.Z);
		UE_LOG(LogClass, Log, TEXT("The Initial Jump: %s"), *JumpString);
		timeString = FString::SanitizeFloat(t01);
		UE_LOG(LogClass, Log, TEXT("JumpMaxHoldTime Variable t01: %s"), *timeString);
		//Once Velocity stops being constant
		JumpString = FString::SanitizeFloat(DeltaX2.Z);
		UE_LOG(LogClass, Log, TEXT("Once the velocity stops being added Jump: %s"), *JumpString);
		timeString = FString::SanitizeFloat(t12);
		UE_LOG(LogClass, Log, TEXT("Time between JumpMax Hold time and when velocity runs out: %s"), *timeString);
		// Total Positive Jump Distance
		JumpString = FString::SanitizeFloat(DeltaX2.Z + DeltaX1.Z);
		UE_LOG(LogClass, Log, TEXT("Total Distance Calculated: %s"), *JumpString);
		timeString = FString::SanitizeFloat(t02);
		UE_LOG(LogClass, Log, TEXT("Total Time till Apex: %s"), *timeString);

		float t20 = FMath::Sqrt(2 * (DeltaX1.Z + DeltaX2.Z) / (MalePlayerMovement->FallingGravityScalar * 980));
		
		timeString = FString::SanitizeFloat(t20);
		UE_LOG(LogClass, Log, TEXT("Time it takes to come back down: %s"), *timeString);


	}
}
void AMalePlayer::JumpActual() {
	FString tmpJumpString;
	float tmpJumpFloat;

	DifferenceInElevation = GetActorLocation() - InitialLocation;
	tmpJumpFloat = DifferenceInElevation.Z;
	UE_LOG(LogClass, Log, TEXT("-----------Actual Jump------------"));

	tmpJumpString = FString::SanitizeFloat(tmpJumpFloat);
	UE_LOG(LogClass, Log, TEXT("Actual Jump Height from start to apex: %s"), *tmpJumpString);
	tmpJumpString = FString::SanitizeFloat(countJumpTime);
	UE_LOG(LogClass, Log, TEXT("Actual time till Apex: %s"), *tmpJumpString);

}
void AMalePlayer::SetupJumpCalculations() {
	JumpCalculated();
	this->JumpMaxHoldTime = JumpTime;
	InitialLocation = GetActorLocation();
	isJumping = true;
}