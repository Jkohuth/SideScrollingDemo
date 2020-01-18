// Fill out your copyright notice in the Description page of Project Settings.


#include "SSDPlayerMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/PhysicsVolume.h"
#include "Components/SplineComponent.h"
#include "Components/CapsuleComponent.h"
#include "SSDCharacter.h"
#include "DrawDebugHelpers.h"



DEFINE_LOG_CATEGORY_STATIC(LogCharacterMovement, Log, All);


USSDPlayerMovementComponent::USSDPlayerMovementComponent(const FObjectInitializer& ObjectInitializer)
	: UCharacterMovementComponent(ObjectInitializer)
{
   // Lock Movement on X Plane
    bConstrainToPlane = true;
    SetPlaneConstraintAxisSetting(LockXAxis);
    bOrientRotationToMovement = true;

    AirControl = NormAirControl;
    JumpZVelocity = NormJumpZVelocity;
    GroundFriction = NormGroundFriction;
    MaxWalkSpeed = NormMaxWalkSpeed;
    MaxFlySpeed = NormMaxFlySpeed; // Why is this here?
	GravityScale = RisingGravityScalar;
	MaxAcceleration = NormMaxAcceleration;
	WallSlideFriction = NormWallSlideFriction; // Pretty Sure I made this variable

	bNotifyApex = true;
	RotationRate = FRotator(0.f, 2160.f, 0.f); // Want snappy turn arounds


	MaxMovementSpeeds = FVector(0.f, MaxWalkSpeed, JumpZVelocity);
	//RailSpeed = FVector(0.f, -500.0f, 0.f);
	KnockBackVelocity = FVector(0.f, 1000.f, 700.f);
	wallSlideVelocity = FVector(0.f, 0.f, -400.f); // Can be changed to be material specific
	jumpWallVelocity = FVector(0.f, 750.f, 1500.f);
}
void USSDPlayerMovementComponent::InitializeComponent() {
    Super::InitializeComponent();
	if (CharacterOwner) {
		capsuleHalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		capsuleRadius = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();
		wallLinesCount = 10.0f; // 30 Lines will be drawn
		wallLinesSpace = (2 * capsuleHalfHeight )/ wallLinesCount;
	}

}

void USSDPlayerMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USSDPlayerMovementComponent::BeginPlay() {
	Super::BeginPlay();
    // Store these locally instead of calling a big function however if they change during a game there coudl be problems
    if (CharacterOwner) {
		capsuleHalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		capsuleRadius = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();
		wallLinesCount = 10.0f; // 30 Lines will be drawn
		wallLinesSpace = (2 * capsuleHalfHeight )/ wallLinesCount;
	}
}

void USSDPlayerMovementComponent::MoveRightInput(float Value){
		
	switch (MovementMode) {
		case MOVE_Custom:
			switch (CustomMovementMode) {
				case ECustomMovementMode::MOVE_Climb:
					break;
			}
			break;
		default:
			AddInputVector(FVector(0.0f, -Value, 0.0f));
			break;
	}
}
void USSDPlayerMovementComponent::MoveUpInput(float Value){

}

void USSDPlayerMovementComponent::SetCustomMovementMode(uint8 CustomMovement) {
	SetMovementMode(MOVE_Custom, CustomMovement);
}
bool USSDPlayerMovementComponent::CheckCustomMovementMode(uint8 CustomMovement){
	return MovementMode == MOVE_Custom && CustomMovementMode == CustomMovement;
}
void USSDPlayerMovementComponent::SetCharacterGravity(float Value){
	GravityScale = Value;
}
void USSDPlayerMovementComponent::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) {

	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	switch (PrevMovementMode) {
	case EMovementMode::MOVE_Custom:
		switch (PreviousCustomMode) {
		case ECustomMovementMode::MOVE_Climb:
//			bSlidingDownWall = false;
			break;
		case ECustomMovementMode::MOVE_Grind:
			MaxAcceleration = MaxAccel;
			break;		
		}
		break;
	case EMovementMode::MOVE_Falling:
		SetCharacterGravity(RisingGravityScalar);
		bNotifyApex = true;
		break;
	case EMovementMode::MOVE_MAX:
		break;
	case EMovementMode::MOVE_None:
		break;
	case EMovementMode::MOVE_Walking:
		break;
	}

}
void USSDPlayerMovementComponent::PhysCustom(float DeltaTime, int32 Iterations) {
	switch (CustomMovementMode) {
	case MOVE_Climb:
		PhysClimb(DeltaTime, Iterations);
		break;
	case MOVE_Grind:
		PhysGrind(DeltaTime, Iterations);
		break;
	case MOVE_Path:
		break;
	default:
		UE_LOG(LogCharacterMovement, Warning, TEXT("Unsupported Movement Mode %d"), int32(MovementMode));
		break;
	}

}

// Climb
void USSDPlayerMovementComponent::PhysClimb(float DeltaTime, int32 Iterations){
    if (DeltaTime < MIN_TICK_TIME) return;

	// Make sure the character own is valid to move if not stop it from moving and return
	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->Role != ROLE_SimulatedProxy))) {
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}
	if (!UpdatedComponent->IsQueryCollisionEnabled()) { SetMovementMode(MOVE_Custom, ECustomMovementMode::MOVE_Climb); return; }

	// This wall sliding mechanic can be simplified to reflect other movement modes such as move flying
	float remainingTime = DeltaTime;
	while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations)) {
		Iterations++;
		bJustTeleported = false;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;

		// Store Current Values
		UPrimitiveComponent *const OldBase = GetMovementBase();
		const FVector PreviousBaseLocation = (OldBase != NULL) ? OldBase->GetComponentLocation() : FVector::ZeroVector;
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();

		RestorePreAdditiveRootMotionVelocity();

		// Ensure velocity is vertical
		const FVector OldVelocity = Velocity;
		
		Velocity = wallSlideVelocity;

		// Apply acceleration
		if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity()) {
			CalcVelocity(timeTick, WallSlideFriction, false, GetMaxBrakingDeceleration());
		}

		// Compute Move Parameters
		const FVector MoveVelocity = Velocity;
		const FVector Delta = timeTick * MoveVelocity;
		const bool bZeroDelta = Delta.IsNearlyZero();

		if (bZeroDelta) {
			remainingTime = 0.f;
		}
		else {
			const FVector DeltaMovement = FVector(0.f, 0.f, MoveVelocity.Z)*DeltaTime;
			FHitResult Hit(1.f);
			SafeMoveUpdatedComponent(DeltaMovement, UpdatedComponent->GetComponentQuat(), true, Hit);

			if (Hit.IsValidBlockingHit()) {
				SetMovementMode(MOVE_Walking);
				StartNewPhysics(remainingTime + timeTick, Iterations - 1);
				return;
			}
		}
		// Check to see if the character is still colliding with the wall
		FHitResult WallHit;
		FVector Start = CharacterOwner->GetActorLocation();
		FVector End = Start;


        // ARROW COMPONENT

		End.Y += (towardWall * (capsuleRadius + climbCheckPadding));

		Start.Z += capsuleHalfHeight;
		End.Z += capsuleHalfHeight;


		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(CharacterOwner);
		bool isHit = GetWorld()->LineTraceSingleByChannel(WallHit, Start, End, ECollisionChannel::ECC_WorldStatic, CollisionParams);
		//DrawDebugLine(GetWorld(), Start, End, FColor::Red, true, 5.f, 0, 5.0f);
		if (!isHit) {
			SetMovementMode(MOVE_Falling);
			StartNewPhysics(remainingTime + timeTick, Iterations - 1);
			return;
		}
	}
}
void USSDPlayerMovementComponent::TriggerClimbMovement(FHitResult ClimbTrigger){

	FHitResult Hit;

	float climbOffset = capsuleRadius + 10.f; //Radius plus some buffer region
	FVector StartTrace = GetActorLocation();
	FVector EndTrace = StartTrace;
	//EndTrace.Y += climbOffset*GetCharacterOwner()->GetActorForwardVector().Y * ClimbTrigger.ImpactNormal.Y;
	towardWall = -1.f*ClimbTrigger.ImpactNormal.Y;// Normal Points away from wall we want towards
	FVector forward = CharacterOwner->GetActorForwardVector();
	FString tmp = "The forward vector" + forward.ToCompactString() + " towardwall" + FString::SanitizeFloat(towardWall);
	if (FMath::IsNearlyEqual(forward.Y,towardWall), 0.2f) {

		FRotator turnAround = CharacterOwner->GetActorRotation();
		turnAround.Yaw += 180.0f;
		CharacterOwner->SetActorRotation(turnAround);
	}
	EndTrace.Y += (capsuleRadius + climbCheckPadding) * towardWall; 
	
	EndTrace.Z += capsuleHalfHeight;
	StartTrace.Z += capsuleHalfHeight; 

	
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(CharacterOwner);
	for(int i = 0; i < wallLinesCount; i++){
		if (GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECollisionChannel::ECC_WorldStatic, CollisionParams)) {
			wallRayCheck.Emplace(true);
		} else
		{
			wallRayCheck.Emplace(false);
		}
		//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, true, 1.0f);

		EndTrace.Z -= wallLinesSpace;
		StartTrace.Z -= wallLinesSpace;
	}
	ClimbCollisionHandler();
}
void USSDPlayerMovementComponent::ClimbCollisionHandler(){
	// We use the array of booleans as a top to bottom list of is there a wall
	// If more than half of the player hit the wall then we can trigger it
	// If the bottom half hit but not the top half "roll"
	Velocity = FVector::ZeroVector;
	SetCustomMovementMode(ECustomMovementMode::MOVE_Climb);
	/*float truePercent = 0.f;
	float falsePercent = 0.f;
	int midpoint = wallLinesSpace /2 ;
	for(int i = 0; i < wallLinesCount; i++){
		wallRayCheck[i] ? (truePercent++) : (falsePercent++);
	}
	truePercent = truePercent / wallLinesCount; // Actually make this a percent
	falsePercent = falsePercent / wallLinesCount;

	if(truePercent >= 0.6f){
		Velocity = FVector::ZeroVector;
		SetCustomMovementMode(ECustomMovementMode::MOVE_Climb);
	} else if(!wallRayCheck[0] && !wallRayCheck[midpoint]){
		// Do Roll
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "You're trying to roll");
	}*/
	wallRayCheck.Empty();
}
void USSDPlayerMovementComponent::PhysGrind(float DeltaTime, int32 Iterations){
	
	if(DeltaTime < MIN_TICK_TIME) return;
	if(!RailSplineReference->IsValidLowLevel()) return;

	if(!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController)){
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}
	if(!UpdatedComponent->IsQueryCollisionEnabled()) { SetMovementMode(MOVE_Custom, ECustomMovementMode::MOVE_Grind); return; }

	float remainingTime = DeltaTime;

	while((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity() || (CharacterOwner->Role))){
		Iterations++;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;

		// Store Current Values
		UPrimitiveComponent* const OldBase = GetMovementBase();
		const FVector PreviousBaseLocation = (OldBase != NULL) ? OldBase->GetComponentLocation() : FVector::ZeroVector;
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		FVector OldVelocity = Velocity;

		RestorePreAdditiveRootMotionVelocity();

		//FVector worldDirAtDist = RailSplineReference->GetDirectionAtDistanceAlongSpline(distanceAlongSpline, ESplineCoordinateSpace::World);

		FVector worldDirAtFeetLoc = RailSplineReference->FindDirectionClosestToWorldLocation(GetActorFeetLocation(), ESplineCoordinateSpace::World);

		float oldDistanceAlongSpline = distanceAlongSpline;

		//NOTES
		// Cosine of the angle the player makes with the rail is the y velocity 
		// Look up NewFallVelocity() for rail calcualtions

		// Lets Calculate gravity along the rail
		// Current the heavy gravity I have may not work well with rail grinding I'm after
		FVector worldDirAtDist = RailSplineReference->GetDirectionAtDistanceAlongSpline(distanceAlongSpline, ESplineCoordinateSpace::World);
		FVector Gravity = FVector(0.f, 0.f, GetGravityZ());
		float GravityMagInDir = FVector::DotProduct(Gravity, worldDirAtDist);
		FVector GravityAlongRail = GravityMagInDir * worldDirAtDist;
		
		//Acceleration = GravityDirAlongRail * worldDirAtDist;
		Velocity += GravityAlongRail * timeTick;
		//Velocity.GetClampedToMaxSize(250.f);

		grindSpeed = FVector::DotProduct(Velocity, worldDirAtDist);

		FString grindString = "The grind speed " + FString::SanitizeFloat(grindSpeed);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, grindString);
		//CalcVelocity(DeltaTime, grindFriction, false, GetMaxBrakingDeceleration());


		FVector beginSpline = RailSplineReference->GetWorldLocationAtSplinePoint(0);
		FVector localBeginSpline = RailSplineReference->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Local);
		FVector endSpline = RailSplineReference->GetWorldLocationAtSplinePoint(RailSplineReference->GetNumberOfSplinePoints());
		FVector localEndSpline = RailSplineReference->GetLocationAtSplinePoint(RailSplineReference->GetNumberOfSplinePoints(), ESplineCoordinateSpace::Local);

		distanceAlongSpline += (grindSpeed * timeTick);

		worldDirAtDist = RailSplineReference->GetDirectionAtDistanceAlongSpline(distanceAlongSpline, ESplineCoordinateSpace::World);
		
		
		/*FVector updatePosition = RailSplineReference->GetWorldLocationAtDistanceAlongSpline(distanceAlongSpline);
		updatePosition.Z += capsuleHalfHeight;
		CharacterOwner->SetActorLocation(updatePosition);

		FVector oldDistWorldLoc = RailSplineReference->GetWorldLocationAtDistanceAlongSpline(oldDistanceAlongSpline);
		FVector newDistWorldLoc = RailSplineReference->GetWorldLocationAtDistanceAlongSpline(distanceAlongSpline);
		FHitResult Hit(1.f);*/

		//FVector UpdateLocation;
		//UpdateLocation = RailSplineReference->GetWorldLocationAtDistanceAlongSpline(distanceAlongSpline);
		//UpdateLocation.Z += capsuleHalfHeight;
		FHitResult Hit(1.f);
		const FVector Adjusted = worldDirAtDist * grindSpeed * timeTick;
		//const FVector Adjusted = Velocity * timeTick;
		//Adjusted = Adjusted * timeTick;

		SafeMoveUpdatedComponent(Adjusted, UpdatedComponent->GetComponentQuat(), true, Hit);
		
		if (Hit.IsValidBlockingHit()) {
			
			if(Hit.GetActor() && !Hit.GetActor()->ActorHasTag(ECustomTags::GrindTag)){
				SetMovementMode(MOVE_Walking);
				StartNewPhysics(remainingTime + timeTick, Iterations - 1);
				return;
			}
			const FVector RequestedAdjustment = GetPenetrationAdjustment(Hit);
			FVector correctionRail = CharacterOwner->GetActorLocation();
			correctionRail.Z += Hit.PenetrationDepth; // Despite being stuck this returns 0
			correctionRail.Z += 2.f;

			CharacterOwner->SetActorLocation(correctionRail);
		}
		if(bJumpOffGrind){
			FVector upVector  = RailSplineReference->GetUpVectorAtDistanceAlongSpline(distanceAlongSpline, ESplineCoordinateSpace::World);
			Velocity.Z += JumpZVelocity;
			float jumpMagnitude = FVector::DotProduct(Velocity, upVector);
			Velocity = jumpMagnitude * upVector;

			bJumpOffGrind = false;
			SetMovementMode(MOVE_Falling);
			StartNewPhysics(remainingTime + timeTick, Iterations - 1);
			return;
		}
		/*if (GetActorFeetLocation().Y > (beginSpline.Y + capsuleRadius) ||
			GetActorFeetLocation().Y < (endSpline.Y - capsuleRadius)) {
			SetMovementMode(MOVE_Falling);
			StartNewPhysics(remainingTime + timeTick, Iterations - 1);
			return;
		}*/
		/*if(distanceAlongSpline >= localEndSpline.X){
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Got To the end of the rail");
			FVector Forward = CharacterOwner->GetActorForwardVector();
			UpdateLocation.Y += (grindSpeed * timeTick * Forward.Y);
			CharacterOwner->SetActorLocation(UpdateLocation);
			if (GetActorFeetLocation().Y < (endSpline.Y - capsuleRadius)) {
				SetMovementMode(MOVE_Falling);
				StartNewPhysics(remainingTime + timeTick, Iterations - 1);
				return;
			}
		}*/


		
		/*FVector OldVelocity = Velocity;
		if(CurrentFloor.HitResult.GetActor())
			FString tmp = CurrentFloor.HitResult.GetActor()->GetName();
	
		FVector Forward = CharacterOwner->GetActorForwardVector();
		FVector RailSplineDirection = RailSplineReference->GetWorldDirectionAtDistanceAlongSpline(distanceAlongSpline);
		float GravityDir = FVector::DotProduct(FVector(0.f, 0.f, GetGravityZ()), RailSplineDirection);
		FVector GravityAccel = GravityDir * RailSplineDirection;
		GravityAccel = FVector(0.f, GravityAccel.Y, GravityAccel.Z);

		if (GravityAccel.Y == 0.f) {
			GravityAccel.Y = Forward.Y * 20.0f;
		}
		Velocity += GravityAccel * timeTick;
		
		FVector gravityTime = GravityAccel * timeTick;
		
		//Apply Gravity
		FRotator splineRotater = RailSplineReference->FindRotationClosestToWorldLocation(GetActorFeetLocation(), ESplineCoordinateSpace::World);
		FHitResult Hit(1.f);
		FVector Adjusted = 0.5f*(Velocity + OldVelocity)*timeTick;

		distanceAlongSpline += Adjusted.SizeSquared() * timeTick;

		SafeMoveUpdatedComponent(Adjusted, UpdatedComponent->GetComponentQuat(), true, Hit);
		
		FString grindString = "Adjusted SafeMoveUpdate " + Adjusted.ToCompactString();
								+ " GravityAcceleration: " + gravityTime.ToCompactString();
		UE_LOG(LogCharacterMovement, Log, TEXT("%s"), *grindString);
		
		// Check if the player Jumped here

		// Make sure the player is still grinding on the rail underneath

		// Set Base to make sure you are not overlapping with the rail

		if(Hit.IsValidBlockingHit()){
			FVector location = GetActorLocation();
			location.Z += 5.f;
			CharacterOwner->SetActorLocation(location);
		}

		FHitResult RailHit;
		FVector Start = GetActorFeetLocation();
		FVector End = Start;
		float tolerance = -10.f;
		End.Z += tolerance;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(CharacterOwner);

		bool isHit = GetWorld()->LineTraceSingleByChannel(RailHit, Start, End, ECollisionChannel::ECC_WorldStatic, CollisionParams);
		if (isHit && RailHit.GetActor() && !RailHit.GetActor()->ActorHasTag(ECustomTags::GrindTag)) {
			SetMovementMode(MOVE_Walking);
			StartNewPhysics(remainingTime + timeTick, Iterations - 1);
			return;
		}
		// Need to check the player has slid off the rail into the air

		int32 NumSplinePts = RailSplineReference->GetNumberOfSplinePoints();
		FVector beginSpline = RailSplineReference->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Local);
		FVector endSpline = RailSplineReference->GetLocationAtSplinePoint(NumSplinePts, ESplineCoordinateSpace::Local);
		FVector LocalCloset = RailSplineReference->FindLocationClosestToWorldLocation(GetActorFeetLocation(), ESplineCoordinateSpace::Local);

		grindString = "Local: " + LocalCloset.ToCompactString() + " endsplinelocal " + endSpline.ToCompactString();
		UE_LOG(LogCharacterMovement, Log, TEXT("%s"), *grindString);

		if(LocalCloset.X == beginSpline.X || LocalCloset.X == endSpline.X){
			SetMovementMode(MOVE_Falling);
			StartNewPhysics(remainingTime + timeTick, Iterations - 1);
			return;
		}*/
	}
}
bool USSDPlayerMovementComponent::IsClimbing() const {
	return (MovementMode == MOVE_Custom) && (CustomMovementMode == MOVE_Climb) && UpdatedComponent;
}
bool USSDPlayerMovementComponent::IsGrinding() const {
	return (MovementMode == MOVE_Custom) && (CustomMovementMode == MOVE_Grind) && UpdatedComponent;
}
void USSDPlayerMovementComponent::TriggerFocusMovement() {
	if (this) {
		AirControl = FocusAirControl * NormAirControl;
		JumpZVelocity = FocusJumpZVelocity * NormJumpZVelocity;
		GroundFriction = FocusGroundFriction * NormGroundFriction;
		MaxWalkSpeed = FocusMaxWalkSpeed * NormMaxWalkSpeed;
		GravityScale = FocusGravityScale * RisingGravityScalar;
		MaxAcceleration = FocusMaxAcceleration * NormMaxAcceleration;
	}



	// I spent this evening doing what I do all day cause I want to bring these ideas to life
	// And I'm not even that good 14 - 1 - 2020
}
void USSDPlayerMovementComponent::HaltFocusMovement() {
	if (this) {
		AirControl = NormAirControl;
		JumpZVelocity = NormJumpZVelocity;
		GroundFriction = NormGroundFriction;
		MaxWalkSpeed = NormMaxWalkSpeed;
		GravityScale = RisingGravityScalar;
		MaxAcceleration = NormMaxAcceleration;
	}

}


void USSDPlayerMovementComponent::BackDash(){

}
void USSDPlayerMovementComponent::KnockBack(const FHitResult& Hit){

}

void USSDPlayerMovementComponent::TriggerGrindMovement(USplineComponent* RailSpline, const FHitResult& RailCollision){
 	if (CheckCustomMovementMode(ECustomMovementMode::MOVE_Grind)) { return; }
	else {
		SetMovementMode(MOVE_Custom, ECustomMovementMode::MOVE_Grind);
		MaxAcceleration = MaxGrindAccel;
		SetCharacterGravity(RisingGravityScalar);
		MaxCustomMovementSpeed = MaxGrindSpeed;
		RailSplineReference = RailSpline;
		
		float playerDistInSplinePt = RailSpline->FindInputKeyClosestToWorldLocation(GetActorFeetLocation());
		float splinePtBeforePlayer = FMath::TruncToFloat(playerDistInSplinePt);
		float splinePtAfterPlayer = splinePtBeforePlayer +1;

		float distanceToSplinePt1 = RailSpline->GetDistanceAlongSplineAtSplinePoint(splinePtBeforePlayer);
		float distanceToSplinePt2 = RailSpline->GetDistanceAlongSplineAtSplinePoint(splinePtAfterPlayer);
		distanceAlongSpline = (playerDistInSplinePt - splinePtBeforePlayer) * (distanceToSplinePt2 - distanceToSplinePt1);
		distanceAlongSpline += distanceToSplinePt1;

		FVector worldLocAtDist = RailSpline->GetWorldLocationAtDistanceAlongSpline(distanceAlongSpline);
		FVector worldDirAtDist = RailSpline->GetDirectionAtDistanceAlongSpline(distanceAlongSpline, ESplineCoordinateSpace::World);
		
		grindSpeed = FVector::DotProduct(Velocity, worldDirAtDist);
		Velocity = grindSpeed * worldDirAtDist;
		worldLocAtDist.Z += capsuleHalfHeight;
		CharacterOwner->SetActorLocation(worldLocAtDist);
		return;
	}
}
void USSDPlayerMovementComponent::OnJumpInput(){
    if (CheckCustomMovementMode(ECustomMovementMode::MOVE_Climb)) {
		JumpOffWall();
	}
	else if (CheckCustomMovementMode(ECustomMovementMode::MOVE_Grind)) {
		bJumpOffGrind = true;
	}
}
void USSDPlayerMovementComponent::JumpOffWall() {
	SetMovementMode(MOVE_Falling);
	Velocity = FVector::ZeroVector;
	Velocity = jumpWallVelocity;
	Velocity.Y *= -1*towardWall;

}
bool USSDPlayerMovementComponent::IsSliding() const {
	return bInSlide;
}
FString USSDPlayerMovementComponent::GetMovementModeString() {
	FString mode;
	switch (MovementMode) {
	case EMovementMode::MOVE_Custom:
		switch (CustomMovementMode) {
		case ECustomMovementMode::MOVE_Climb:
			mode = "MOVE_Climb";
			//			bSlidingDownWall = false;
			break;
		case ECustomMovementMode::MOVE_Grind:
			mode = "MOVE_Grind";
			break;
		}
		break;
	case EMovementMode::MOVE_Falling:
		mode = "MOVE_Falling";
		break;
	case EMovementMode::MOVE_MAX:
		break;
	case EMovementMode::MOVE_None:
		mode = "MOVE_None";
		break;
	case EMovementMode::MOVE_Walking:
		mode = "MOVE_Walking";
		break;
	default:
		mode = "Other";
			break;
	}
	return mode;
}