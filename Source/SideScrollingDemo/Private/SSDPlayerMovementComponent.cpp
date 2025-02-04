// Fill out your copyright notice in the Description page of Project Settings.


#include "SSDPlayerMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/PhysicsVolume.h"
#include "Components/SplineComponent.h"
#include "Components/CapsuleComponent.h"
#include "SSDCharacter.h"
#include "DrawDebugHelpers.h"
#include "Updraft.h"
#include "Engine.h"
#include "Math/UnrealMathUtility.h"
#include "Rail.h"


DEFINE_LOG_CATEGORY_STATIC(LogCharacterMovement, Log, All);


USSDPlayerMovementComponent::USSDPlayerMovementComponent(const FObjectInitializer& ObjectInitializer)
	: UCharacterMovementComponent(ObjectInitializer)
{
   // Lock Movement on X Plane
    bConstrainToPlane = true;
    SetPlaneConstraintAxisSetting(LockXAxis);
    bOrientRotationToMovement = true;

    /*AirControl = NormAirControl;
    JumpZVelocity = NormJumpZVelocity;
    GroundFriction = NormGroundFriction;
    MaxWalkSpeed = NormMaxWalkSpeed;
    MaxFlySpeed = NormMaxFlySpeed; // Why is this here?
	MaxAcceleration = NormMaxAcceleration;
	WallSlideFriction = NormWallSlideFriction; // Pretty Sure I made this variable
	*/
	UpdateCharacterMovementValues(NormalPlayerMovement);

	bNotifyApex = true;
	RotationRate = FRotator(0.f, 2160.f, 0.f); // Want snappy turn arounds
	GravityScale = RisingGravityScalar;

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

	if (IsUpdrafting()) UpdateUpdraftMovement(DeltaTime);
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
				case ECustomMovementMode::MOVE_Swing:
					//if (pivotPosition.Z > GetActorLocation().Z) {
						AddInputVector(FVector(0.f, -Value, 0.f));
					//}

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
void USSDPlayerMovementComponent::OnJumpInput() {

	if (CheckCustomMovementMode(ECustomMovementMode::MOVE_Climb)) {
		JumpOffWall();
	}
	else if (CheckCustomMovementMode(ECustomMovementMode::MOVE_Grind)) {
		bJumpOffGrind = true;
	}
	else if (CheckCustomMovementMode(ECustomMovementMode::MOVE_Swing) || isSwinging) {
		HaltSwingMovement();

	}
}
void USSDPlayerMovementComponent::SetCustomMovementMode(uint8 CustomMovement) {
	SetMovementMode(MOVE_Custom, CustomMovement);
}
bool USSDPlayerMovementComponent::CheckCustomMovementMode(uint8 CustomMovement){
	return MovementMode == MOVE_Custom && CustomMovementMode == CustomMovement;
}
bool USSDPlayerMovementComponent::CheckMovementMode(uint8 ExpectedMovement) {
	return MovementMode == ExpectedMovement;
}
void USSDPlayerMovementComponent::SetCharacterGravity(float Value){
	GravityScale = Value;
}
void USSDPlayerMovementComponent::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) {
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	FString PreviousMovementModeString = "Previous ModementMode ";

	switch (PrevMovementMode) {
	case EMovementMode::MOVE_Custom:
		switch (PreviousCustomMode) {
		case ECustomMovementMode::MOVE_Climb:
//			bSlidingDownWall = false;
			break;
		case ECustomMovementMode::MOVE_Grind:
			MaxAcceleration = MaxAccel;
			break;		
		case ECustomMovementMode::MOVE_Swing:
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
	case MOVE_Swing:
		PhysSwing(DeltaTime, Iterations);
		break;
	default:
		UE_LOG(LogCharacterMovement, Warning, TEXT("Unsupported Movement Mode %d"), int32(MovementMode));
		break;
	}

}
void USSDPlayerMovementComponent::UpdateCharacterMovementValues(FPlayerMovementHandler newPlayerMovementValues) {
	AirControl = newPlayerMovementValues.AirControl;
	JumpZVelocity = newPlayerMovementValues.JumpZVelocity;
	GroundFriction = newPlayerMovementValues.GroundFriction;
	MaxWalkSpeed = newPlayerMovementValues.MaxWalkSpeed;
	MaxAcceleration = newPlayerMovementValues.MaxAcceleration;

	WallSlideJumpSpeed = newPlayerMovementValues.WallSlideJumpSpeed;
	WallSlideFriction = newPlayerMovementValues.WallSlideFriction;
	
	GrindJumpSpeed = newPlayerMovementValues.GrindJumpSpeed;
	GrindMaxAccel = newPlayerMovementValues.GrindMaxAccel;
	GrindMaxSpeed = newPlayerMovementValues.GrindMaxSpeed;
	GrindFriction = newPlayerMovementValues.GrindFriction;

	SwingMaxAccel = newPlayerMovementValues.SwingMaxAccel;
	SwingMaxSpeed = newPlayerMovementValues.SwingMaxSpeed;
	SwingFriction = newPlayerMovementValues.SwingFriction;

}

// Climb
void USSDPlayerMovementComponent::PhysClimb(float DeltaTime, int32 Iterations){
    if (DeltaTime < MIN_TICK_TIME) return;

	// Make sure the character own is valid to move if not stop it from moving and return
	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy))) {
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
		TArray<bool> wallTraces;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(CharacterOwner);
        // ARROW COMPONENT
		End.Y += (towardWall * (capsuleRadius + climbCheckPadding));

		Start.Z += capsuleHalfHeight;
		End.Z += capsuleHalfHeight;


		wallTraces.Add(GetWorld()->LineTraceSingleByChannel(WallHit, Start, End, ECollisionChannel::ECC_WorldStatic, CollisionParams));

		Start.Z -= capsuleHalfHeight;
		End.Z -= capsuleHalfHeight;

		wallTraces.Add(GetWorld()->LineTraceSingleByChannel(WallHit, Start, End, ECollisionChannel::ECC_WorldStatic, CollisionParams));

		Start.Z -= capsuleHalfHeight;
		End.Z -= capsuleHalfHeight;

		wallTraces.Add(GetWorld()->LineTraceSingleByChannel(WallHit, Start, End, ECollisionChannel::ECC_WorldStatic, CollisionParams));


		//DrawDebugLine(GetWorld(), Start, End, FColor::Red, true, 5.f, 0, 5.0f);
		if (!wallTraces[0] && !wallTraces[1] && !wallTraces[2]) {
			SetMovementMode(MOVE_Falling);
			StartNewPhysics(remainingTime + timeTick, Iterations - 1);
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "No longer on wall");
			return;
		}
	}
}
void USSDPlayerMovementComponent::TriggerClimbMovement(FHitResult ClimbTrigger){

	FHitResult Hit;

	// Since this is a 2D game I can cut a quick corner. Actor Rotation needs to either be FRotator(0.f, -90.f, 0.f) or FRotator(0.f, 90.f, 0.f) based on the impact point

	// The actor rotation to always face away from the way needs needs to be the opposite of the impact normal point

	if (ClimbTrigger.ImpactNormal.Y == 1) {
		CharacterOwner->SetActorRotation(FRotator(0.f, 90.f,0.f));

	}
	else if (ClimbTrigger.ImpactNormal.Y == -1) {
		CharacterOwner->SetActorRotation(FRotator(0.f, -90.f, 0.f));
	}

	towardWall = -1.f*ClimbTrigger.ImpactNormal.Y;// Normal Points away from wall we want towards

	/*float climbOffset = capsuleRadius + 10.f; //Radius plus some buffer region
	FVector StartTrace = GetActorLocation();
	FVector EndTrace = StartTrace;
	FVector forward = CharacterOwner->GetActorForwardVector();
	FString tmp = "The forward vector" + forward.ToCompactString() + " towardwall" + FString::SanitizeFloat(towardWall);
	if (FMath::IsNearlyEqual(forward.Y,towardWall), 0.2f) {

		FRotator turnAround = CharacterOwner->GetActorRotation();
		// Get the forward vector from the arrow component and make it point away from the wall CharacterOwner->GetArrowComponent()->GetForwardVector();
		turnAround.Yaw += 180.0f;
		CharacterOwner->SetActorRotation(turnAround);
	}
	EndTrace.Y += (capsuleRadius + climbCheckPadding) * towardWall; 
	
	EndTrace.Z += capsuleHalfHeight;
	StartTrace.Z += capsuleHalfHeight; 

	// This was started in good faith to have the character roll onto ledges but I think its better to not use this for the demo

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
	}*/
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
	}
	wallRayCheck.Empty();*/
}
void USSDPlayerMovementComponent::PhysGrind(float DeltaTime, int32 Iterations){
	
	if(DeltaTime < MIN_TICK_TIME) return;
	if(!RailSplineReference->IsValidLowLevel()) return;

	FString debugGrind;

	float remainingTime = DeltaTime;
	while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations)) {
		Iterations++;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;

		// Store Current Values
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FQuat PawnRotation = UpdatedComponent->GetComponentQuat();
		bJustTeleported = false;

		FVector ActorForwardVector = CharacterOwner->GetActorForwardVector();

		RestorePreAdditiveRootMotionVelocity();

		const FVector OldVelocity = Velocity;

		// Stash the current distance along spline
		float oldDistanceAlongSpline = distanceAlongSpline;

		// Apply Input 
		// ******************
		// I do want input to effect Acceleration but this can be done later
		// ******************

		debugGrind = "";

		// Compute Current Gravity - Use dot product to find Gravity along the angle of the rail
		//FVector worldDirAtDist = RailSplineReference->GetDirectionAtDistanceAlongSpline(distanceAlongSpline, ESplineCoordinateSpace::World);
		FVector worldDirAtDist = RailSplineReference->FindDirectionClosestToWorldLocation(GetActorFeetLocation(), ESplineCoordinateSpace::World);
		
		FVector Gravity = FVector(0.f, 0.f, GetGravityZ());
		float GravityMagInDir = FVector::DotProduct(Gravity, worldDirAtDist);
		FVector GravityAlongRail = GravityMagInDir * worldDirAtDist;

		debugGrind += "WorldDirAtDist " + worldDirAtDist.ToCompactString() + " GravityMag " + FString::SanitizeFloat(GravityMagInDir) + " Gravity along rail " + GravityAlongRail.ToCompactString();

		// Calculate Velocity - currently there is no terminal limit
		Velocity += GravityAlongRail * timeTick;

		// Ensure the Velocity is along the rail
		Velocity = worldDirAtDist * FVector::DotProduct(Velocity, worldDirAtDist);


		// Require a minimum velocity so character doesn't stand stationary
		if (FMath::IsNearlyEqual(GravityMagInDir, 0.f, 2.f) && Velocity.Equals(FVector::ZeroVector, minGrindVelocity.Size())) {
			Velocity = minGrindVelocity * ActorForwardVector;
		}

		FHitResult Hit(1.f);
		FVector Adjusted = 0.5f*(OldVelocity + Velocity) *timeTick;

		debugGrind += " Velocity " + Velocity.ToCompactString() + " OldVelocity " + OldVelocity.ToCompactString()+" Adjusted " + Adjusted.ToCompactString();

		SafeMoveUpdatedComponent(Adjusted, PawnRotation, true, Hit);

		debugGrind = "";
		debugGrind += "Hit Component Information: " + Hit.ToString();
		

		LogAtReducedRate(debugGrind, 5);
		
		if (Hit.GetActor() && Hit.GetActor()->ActorHasTag(ECustomTags::GrindTag)) {
			distanceAlongSpline = GetDistanceAlongSpline(RailSplineReference);
			FVector location = RailSplineReference->GetWorldLocationAtDistanceAlongSpline(distanceAlongSpline);
			location.Z += capsuleHalfHeight + railRadius;
			CharacterOwner->SetActorLocation(location);
			ARail* rail = Cast<ARail>(Hit.GetActor());
			if (rail) {
				continue;
			}
			
		}
/*
		// Express Velocity as a speed along the rail
		grindSpeed = FVector::DotProduct(Velocity, worldDirAtDist);


		//NOTES
		// Cosine of the angle the player makes with the rail is the y velocity 
		// Look up NewFallVelocity() for rail calcualtions

		// Lets Calculate gravity along the rail
		// Current the heavy gravity I have may not work well with rail grinding I'm after

		//FString grindString = "The grind speed " + FString::SanitizeFloat(grindSpeed);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, grindString);
		//CalcVelocity(DeltaTime, grindFriction, false, GetMaxBrakingDeceleration());

		distanceAlongSpline += (grindSpeed * timeTick);

		worldDirAtDist = RailSplineReference->GetDirectionAtDistanceAlongSpline(distanceAlongSpline, ESplineCoordinateSpace::World);


		FVector beginSpline = RailSplineReference->GetWorldLocationAtSplinePoint(0);
		FVector localBeginSpline = RailSplineReference->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Local);
		FVector endSpline = RailSplineReference->GetWorldLocationAtSplinePoint(RailSplineReference->GetNumberOfSplinePoints());
		FVector localEndSpline = RailSplineReference->GetLocationAtSplinePoint(RailSplineReference->GetNumberOfSplinePoints(), ESplineCoordinateSpace::Local);

		// Only works with thin rails
		FVector updatedGrindLocation;
		updatedGrindLocation = RailSplineReference->GetWorldLocationAtDistanceAlongSpline(distanceAlongSpline);

		// If we reached the end of the spline we need to get the next world location manually
		//FString locations = "updatedGrindLocation: " + updatedGrindLocation.ToCompactString() + "\nBeginSpline: " + beginSpline.ToCompactString() + "\nEndspline: " + endSpline.ToCompactString();
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *locations);

		if (updatedGrindLocation.Equals(beginSpline, capsuleRadius) || updatedGrindLocation.Equals(endSpline,capsuleRadius)) {
			//Velocity = FMath::Sign(Velocity.Y) * worldDirAtDist * Velocity;

			updatedGrindLocation = CharacterOwner->GetActorLocation();
			updatedGrindLocation += Velocity * timeTick;
		}
		else {
			updatedGrindLocation.Z += capsuleHalfHeight;
		}
		//CharacterOwner->SetActorLocation(updatedGrindLocation);


		FVector localPlayerLocationAlongSpline = RailSplineReference->FindLocationClosestToWorldLocation(GetActorFeetLocation(), ESplineCoordinateSpace::Local);
		*/
		if (!HasValidData()) return;

		//Should be in a struct thats initiated with the spline component

		if(bJumpOffGrind){
			FVector upVector  = RailSplineReference->GetUpVectorAtDistanceAlongSpline(distanceAlongSpline, ESplineCoordinateSpace::World);
			Velocity.Z += GrindJumpSpeed;
			float jumpMagnitude = FVector::DotProduct(Velocity, upVector);
			Velocity = jumpMagnitude * upVector;

			bJumpOffGrind = false;
			SetMovementMode(MOVE_Falling);
			StartNewPhysics(remainingTime + timeTick, Iterations - 1);
			return;
		}
/*
		if (localPlayerLocationAlongSpline.Equals(localBeginSpline, capsuleRadius) ||
			localPlayerLocationAlongSpline.Equals(localEndSpline, capsuleRadius)) {
			FHitResult HitCenter;
			FHitResult HitBehind;
			FHitResult HitForward;

			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(CharacterOwner);

			float endTraceZ = 10.f;
			float endTraceCapsuleZ = 5.f;
			float capsuleRadiusIncrease = 5.f;
			FVector StartTrace = GetActorFeetLocation();
			//UE_LOG(LogCharacterMovement, Log, TEXT("GetActorFeetLocation %s"), *GetActorFeetLocation().ToCompactString());
			//StartTrace.Y += capsuleRadius * FMath::Sign(Velocity.Y);
			FVector EndTrace = StartTrace;
			EndTrace.Z -= endTraceZ;

			// Perhaps replace this with shape trace
			DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red);
			GetWorld()->LineTraceSingleByChannel(HitCenter, StartTrace, EndTrace, ECollisionChannel::ECC_WorldStatic, CollisionParams);
			
			//StartTrace.Z += capsuleRadius;

			StartTrace.Y += capsuleRadius * -1 * ActorForwardVector.Y;
			EndTrace.Y = StartTrace.Y;
			DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red);
			GetWorld()->LineTraceSingleByChannel(HitBehind, StartTrace, EndTrace, ECollisionChannel::ECC_WorldStatic, CollisionParams);

			StartTrace.Y += 2 * capsuleRadius * ActorForwardVector.Y;
			EndTrace.Y = StartTrace.Y;
			DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red);
			GetWorld()->LineTraceSingleByChannel(HitBehind, StartTrace, EndTrace, ECollisionChannel::ECC_WorldStatic, CollisionParams);


			// Collision Shape trace in the future will be superior way of checking
			FCollisionShape scanCapsule = FCollisionShape::MakeCapsule(FVector(capsuleRadius, capsuleRadius + capsuleRadiusIncrease, 1.f));
			StartTrace = GetActorFeetLocation();
			EndTrace = StartTrace;
			EndTrace.Z -= endTraceZ;

			//DrawDebugCapsule(GetWorld(), GetActorFeetLocation(), scanCapsule.GetCapsuleHalfHeight(), scanCapsule.GetCapsuleRadius(), FQuat::Identity, FColor::Blue, false, 1.0f);

			TArray<FHitResult> HitResults;
			GetWorld()->SweepMultiByChannel(HitResults, StartTrace, EndTrace, FQuat::Identity, ECollisionChannel::ECC_WorldStatic, scanCapsule);

			bool hitFloor = false;

			for (auto HitResult : HitResults) {
				if (HitResult.bBlockingHit && HitResult.GetActor() && HitResult.GetActor()->ActorHasTag(ECustomTags::GrindTag)) {
					return;
				}
				else if (HitResult.bBlockingHit && HitResult.GetActor()) {
					hitFloor = true;
				}
			}
			if (hitFloor) {
				SetMovementMode(MOVE_Walking);
				StartNewPhysics(remainingTime + timeTick, Iterations - 1);
				return;
			}
			else {
				SetMovementMode(MOVE_Falling);
				StartNewPhysics(remainingTime + timeTick, Iterations - 1);
				return;
			}
			// Directly underneath player and behind player, if hit something that is not a rail start walking
			/*if( (HitCenter.bBlockingHit && HitCenter.GetActor() && !HitCenter.GetActor()->ActorHasTag(ECustomTags::GrindTag)) &&
				(HitBehind.bBlockingHit && HitBehind.GetActor() && !HitBehind.GetActor()->ActorHasTag(ECustomTags::GrindTag)) &&
				(HitForward.bBlockingHit && HitForward.GetActor() && !HitForward.GetActor()->ActorHasTag(ECustomTags::GrindTag)) ) {
				// Directly underneath the character is not a rail check the radius behind
				SetMovementMode(MOVE_Walking);
				StartNewPhysics(remainingTime + timeTick, Iterations - 1);
				return;

			}
			// We didn't hit anything he fell off the rail
			else if (!HitCenter.bBlockingHit && !HitBehind.bBlockingHit && !HitForward.bBlockingHit) {
				// Make sure that capsule is over the edge
				// This should be done in a large for loop
				SetMovementMode(MOVE_Falling);
				StartNewPhysics(remainingTime + timeTick, Iterations - 1);
				return;
			}
	
		}*/
 	}
}

void USSDPlayerMovementComponent::TriggerGrindMovement(ARail* Rail, const FHitResult& RailCollision) {
	if (CheckCustomMovementMode(ECustomMovementMode::MOVE_Grind)) { return; }
	else {
		SetMovementMode(MOVE_Custom, ECustomMovementMode::MOVE_Grind);
		MaxAcceleration = GrindMaxAccel;
		SetCharacterGravity(RisingGravityScalar);
		MaxCustomMovementSpeed = GrindMaxSpeed;
		RailSplineReference = Rail->GetRailSpline();

		distanceAlongSpline = GetDistanceAlongSpline(RailSplineReference);

		FVector worldLocAtDist = RailSplineReference->GetWorldLocationAtDistanceAlongSpline(distanceAlongSpline);
		FVector worldDirAtDist = RailSplineReference->GetDirectionAtDistanceAlongSpline(distanceAlongSpline, ESplineCoordinateSpace::World);

		grindSpeed = FVector::DotProduct(Velocity, worldDirAtDist);
		Velocity = grindSpeed * worldDirAtDist;

		railRadius = Rail->GetRailRadius();  // Since I am no longer using SetActorLocation and am instead using SafeMoveUpdate I think the collisions are holding me back
		worldLocAtDist.Z += capsuleHalfHeight;
		CharacterOwner->SetActorLocation(worldLocAtDist);

		FString triggerGrind = "Velocity At Grind Initialization: " + Velocity.ToCompactString() + " WorldDirection: " + worldDirAtDist.ToCompactString() + " RailRadius " + FString::SanitizeFloat(railRadius) ;
		LogAtReducedRate(triggerGrind, 1);
		return;
	}
}
float USSDPlayerMovementComponent::GetDistanceAlongSpline(USplineComponent* SplineComponent) {

	float playerDistInSplinePt = SplineComponent->FindInputKeyClosestToWorldLocation(GetActorFeetLocation());
	float splinePtBeforePlayer = FMath::TruncToFloat(playerDistInSplinePt);
	float splinePtAfterPlayer = splinePtBeforePlayer + 1;

	float distanceToSplinePt1 = SplineComponent->GetDistanceAlongSplineAtSplinePoint(splinePtBeforePlayer);
	float distanceToSplinePt2 = SplineComponent->GetDistanceAlongSplineAtSplinePoint(splinePtAfterPlayer);

	return ((playerDistInSplinePt - splinePtBeforePlayer) * (distanceToSplinePt2 - distanceToSplinePt1)) + distanceToSplinePt1;
}


bool USSDPlayerMovementComponent::IsClimbing() const {
	return (MovementMode == MOVE_Custom) && (CustomMovementMode == MOVE_Climb) && UpdatedComponent;
}
bool USSDPlayerMovementComponent::IsGrinding() const {
	return (MovementMode == MOVE_Custom) && (CustomMovementMode == MOVE_Grind) && UpdatedComponent;
}
void USSDPlayerMovementComponent::TriggerFocusMovement() {
	if (this) {
		UpdateCharacterMovementValues(FocusPlayerMovement);
	}

	// I spent this evening doing what I do all day cause I want to bring these ideas to life
	// And I'm not even that good 14 - 1 - 2020
}
void USSDPlayerMovementComponent::TriggerUpdraftMovement(AUpdraft* updraft) {
	bUpdraft = true;
	SetMovementMode(MOVE_Falling);
	updraftSpeed = updraft->GetDraftSpeed(GetActorFeetLocation(), GravityScale);
	FString tmp = "Updraft speed on trigger " + FString::SanitizeFloat(updraftSpeed);
	tmp += " GravityScale " + FString::SanitizeFloat(GravityScale);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *tmp);
	UpdraftReference = updraft;

	// set bUpdraft to true
	// set movementmode to falling
	// Call the updraft component
	// store the reference locally
	// Add updraft velocity to the vertical component

}
void USSDPlayerMovementComponent::HaltUpdraftMovement() {
	bUpdraft = false;

	// Might be dangerous
	//UpdraftReference = nullptr;
	// called on overlap end
	// set bUpdraft to false
	// clear updraft component
}
void USSDPlayerMovementComponent::UpdateUpdraftMovement(float DeltaTime) {

	 updraftSpeed = UpdraftReference->GetDraftSpeed(GetActorFeetLocation(), GravityScale);


	 if (Velocity.Z <= updraftSpeed) {
		 Velocity.Z += updraftSpeed * DeltaTime;
		 FString print = "GravityScalar " + FString::SanitizeFloat(GravityScale)
			 + " UpdraftSpeed " + FString::SanitizeFloat(updraftSpeed) + " Gravity " + FString::SanitizeFloat(GetGravityZ());
		 //PrintStringToScreen(print);
	}
	// Don't let player velocity drop down below the threshold
	// Check if the player has moved up to the less strong wind threshold
}
void USSDPlayerMovementComponent::PrintStringToScreen(FString print) {
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, print);

}
void USSDPlayerMovementComponent::HaltFocusMovement() {
	// Update this you swine
	if (this) {
		UpdateCharacterMovementValues(NormalPlayerMovement);
	}

}

// Swing

void USSDPlayerMovementComponent::TriggerSwingMovement(FVector pivotPosition) {
	if (!CheckMovementMode(MOVE_Falling)) return; 
	SetMovementMode(MOVE_Custom, ECustomMovementMode::MOVE_Swing);

	// Store the Location of the Pivot in a Local Variable for use later
	this->pivotPosition = pivotPosition;
	
	CharacterOwner->SetActorLocation(CalculateCharacterPivotDistance(pivotPosition));

	FString triggerSwingString = "Max Acceleration Pre " + FString::SanitizeFloat(GetMaxAcceleration());// "Velocity At Swing Initialization " + Velocity.ToCompactString();
	MaxAcceleration = SwingMaxAccel;
	triggerSwingString += " Max Acceleration Post " + FString::SanitizeFloat(GetMaxAcceleration()) + " Swing Accel " + FString::SanitizeFloat(SwingMaxAccel);
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, triggerSwingString);
	FVector dirPlayerPivot = this->pivotPosition - CharacterOwner->GetActorLocation();
	horizontalDistanceFromPivot = dirPlayerPivot.Y;
	
	
	Velocity = Velocity.Size() * dirPlayerPivot.GetSafeNormal();
	Velocity = Velocity.GetClampedToMaxSize(SwingMaxSpeed);
	//triggerSwingString = " Velocity After swing " + Velocity.ToCompactString() + " DirPivotGetSafeNormal "  + dirPlayerPivot.GetSafeNormal().ToCompactString();


	
	bOrientRotationToMovement = false;
	bUseControllerDesiredRotation = false;
	// This does the job but the character still wants to rotate
	//RotationRate = FRotator::ZeroRotator;
	// TODO: Find the sum of forces (Gravity and Tension) here and then get the velocity using the dot product against that
	//Velocity = FVector::ZeroVector;

	//FString PhysSwingLogging = "Pivot Postiion " + pivotPosition.ToCompactString() + " Theta: " + FString::SanitizeFloat(theta) + " Sin(theta) " + FString::SanitizeFloat(FMath::Sin(theta)) + " Cos(theta) " + FString::SanitizeFloat(FMath::Cos(theta)) + " theta degrees " + FString::SanitizeFloat(FMath::RadiansToDegrees(theta)) + " updatePos " + updatePos.ToCompactString() + " Updated Location " + newLocation.ToCompactString();
	//UE_LOG(LogCharacterMovement, Log, TEXT("%s"), *PhysSwingLogging);

}

void USSDPlayerMovementComponent::PhysSwing(float DeltaTime, int32 Iterations) {
	if (DeltaTime < MIN_TICK_TIME) return;

	FString debugSwing;

	float remainingTime = DeltaTime;
	while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations)) {
		Iterations++;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;

		// Store Current Values
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FQuat PawnRotation = UpdatedComponent->GetComponentQuat();
		bJustTeleported = false;

		const FVector OldVelocity = Velocity;
		float MaxDecel = GetMaxBrakingDeceleration();

		bool debugLog = false;


		// TODO: 
		// Get Player Input for horizontal movement
		// Add acceleration in direction of the forces (Tension + Gravity)
		debugSwing = "";
		debugSwing += "Acceleration " + Acceleration.ToCompactString() + " Velocity " + Velocity.ToCompactString();
		CalcVelocity(timeTick, SwingFriction, false, MaxDecel); // Using this function can I set a resonable max acceleration
		debugSwing += " Post Accel " + Acceleration.ToCompactString() + " Post Vel " + Velocity.ToCompactString();

		FVector Gravity(0.f, 0.f, GetGravityZ());
			
		//debugSwing += "Velocity init: " + Velocity.ToCompactString() + " Gravity: " + Gravity.ToCompactString();

		FVector dirPlayerPivot = pivotPosition - CharacterOwner->GetActorLocation();
		horizontalDistanceFromPivot = dirPlayerPivot.Y;

		float theta = GetAngleForSwing(dirPlayerPivot);
		FVector Tension = FVector::ZeroVector;
		float centripAccelMag = FMath::Pow(Velocity.Size(), 2) / lengthOfPendulum;
		Tension = (FMath::Abs(GetGravityZ())*FMath::Cos(theta) + centripAccelMag) * dirPlayerPivot.GetSafeNormal();

		if (GetActorLocation().Z >= pivotPosition.Z) {
			Tension = FVector::ZeroVector;
			//Velocity.Z -= 20.f;
			//Velocity = FVector::ZeroVector;
			//debugSwing += " ThetaDegrees: " + FString::SanitizeFloat(FMath::RadiansToDegrees(theta)) + " ThetaRadians " + FString::SanitizeFloat(theta);
			Velocity.Y -= Velocity.Y /2.f;
			debugLog = true;
		}
		//debugSwing += " Tension: " + Tension.ToCompactString() + " theta Degrees: " + FString::SanitizeFloat(FMath::RadiansToDegrees(theta)) + " dirPlayerPivot " + dirPlayerPivot.ToCompactString();
		//debugSwing += " Velocity After Gravity: " + Velocity.ToCompactString() +  " Direction of TensionVec: " + dirPlayerPivot.GetSafeNormal().ToCompactString() + " Tension " + Tension.ToCompactString() + " timeTick " + FString::SanitizeFloat(timeTick);

		Velocity = NewSwingVelocity(Velocity, Gravity, Tension, timeTick);
		if (debugLog) {
		//	debugSwing += " NewVel " + Velocity.ToCompactString() + " OldVel " + OldVelocity.ToCompactString() + " Gravity " + Gravity.ToCompactString() + " Tension " + Tension.ToCompactString();
		}

		//debugSwing += "Velocity Size" + FString::SanitizeFloat(Velocity.Size()) + " Velocity + Accelerations: " + Velocity.ToCompactString();

		//debugSwing += " ThetaDegrees: " + FString::SanitizeFloat(FMath::RadiansToDegrees(theta)) + " ThetaRadians " + FString::SanitizeFloat(theta) + " centripAccel: " + FString::SanitizeFloat(centripAccelMag) + " Tension: " + Tension.ToCompactString();

		FVector Adjusted = 0.5f * (OldVelocity + Velocity) * timeTick;

		//debugSwing += " Adjusted " + Adjusted.ToCompactString();

		FHitResult Hit(1.f);
		SafeMoveUpdatedComponent(Adjusted, PawnRotation, true, Hit);
		dirPlayerPivot = pivotPosition - CharacterOwner->GetActorLocation();

		FVector CharacterDistance = CalculateCharacterPivotDistance(pivotPosition);

		if (!dirPlayerPivot.Equals(CharacterDistance, 30.f)) {
			CharacterOwner->SetActorLocation(CharacterDistance);
		}

		//theta = CalculateAngleCharacterPivot(pivotPosition);

		//reduceLogging++;
		if (reduceLogging % 1 == 0 && debugLog) {
			UE_LOG(LogCharacterMovement, Log, TEXT("%s"), *debugSwing);
			reduceLogging = 1;
			debugLog = false;
		}

	}
}
void USSDPlayerMovementComponent::HaltSwingMovement() {
	SetMovementMode(MOVE_Falling);
	isSwinging = false;
	this->pivotPosition = FVector::ZeroVector;
	FString str = "Current Velocity " + Velocity.ToCompactString();
	LogAtReducedRate(str, 1);
	bOrientRotationToMovement = true;
	bUseControllerDesiredRotation = true;
	Velocity.Z += JumpVelocity;
	//CharacterOwner->SetActorRotation(actorRotationPreSwing);

}
float USSDPlayerMovementComponent::GetAngleForSwing(FVector DirectionVector) {
	//if (DirectionVector.Y <= KINDA_SMALL_NUMBER){
	//	return FMath::Atan(0.f);
	//}
	return FMath::Atan(DirectionVector.Y / DirectionVector.Z);
}
FVector USSDPlayerMovementComponent::CalculateCharacterPivotDistance(FVector pivotPosition) {

	FVector playerPosition = CharacterOwner->GetActorLocation();

	float hyp = FVector::Dist(pivotPosition, playerPosition);
	float opp = FMath::Abs(pivotPosition.Z - playerPosition.Z);
	float theta = FMath::Asin(opp / hyp);

	// Now that we have theta for a triangle in quad 1 lets find where it really is
	// Lets break this down into two Y and Z
	//                                                           ^ Z
	//                                                           |
	// if player Y > pivot Y under my really dumb system Y <______ That means quad 2/3
	// if player Z > pivot Z 

	if (playerPosition.Z >= pivotPosition.Z) {
		if (playerPosition.Y <= pivotPosition.Y) {
			// quad 1
			theta += 0.f;
		}
		else if (playerPosition.Y >= pivotPosition.Y) {
			// quad 2
			theta = PI - theta;
		}
	}
	else if (playerPosition.Z <= pivotPosition.Z) {
		if (playerPosition.Y >= pivotPosition.Y) {
			// quad 3
			theta += PI;
		}
		else if (playerPosition.Y <= pivotPosition.Y) {
			// quad 1
			theta = 2 * PI - theta;
		}
	}

	FVector updatePos = FVector::ZeroVector;

	updatePos.Y = lengthOfPendulum * FMath::Cos(theta);
	updatePos.Z = lengthOfPendulum * FMath::Sin(theta);


	// if actorLocation.Z > pivotLocation.Z quad 1 or 2
	// if actorLocation.Y > pivotlocation.Y quat 2 or 3 fucking level is backwards

	FVector newLocation = FVector(0.f, pivotPosition.Y - updatePos.Y, pivotPosition.Z + updatePos.Z);
	return newLocation;
}

FVector USSDPlayerMovementComponent::NewSwingVelocity(const FVector& InitialVelocity, const FVector& Gravity, const FVector& Tension, float DeltaTime) const {
	FVector Result = InitialVelocity;

	FVector SumOfForces = Gravity + Tension;

	if (DeltaTime > 0.f) {
		//Result += Gravity * DeltaTime;
		//Result += Tension * DeltaTime;
		Result += SumOfForces * DeltaTime;
		// Don't exceed terminal velocity.
		//const float TerminalLimit = FMath::Abs(GetPhysicsVolume()->TerminalVelocity);
		const float TerminalLimit = SwingMaxSpeed;
		if (Result.SizeSquared() > FMath::Square(TerminalLimit))
		{

			const FVector SumOfForcesDir = Result / Result.Size(); // I don't get why I have to make my own unit direction vector
			

			Result = SumOfForcesDir * TerminalLimit; // I've decided to clamp it and see what happens
			//UE_LOG(LogClass, Log, TEXT("2 NewSwingVelocity %s and this is the SumOfForcesDir %s"), *Result.ToCompactString(), *SumOfForcesDir.ToCompactString());
		}
	}
	return Result;
}

bool USSDPlayerMovementComponent::IsSwinging() const {
	return (MovementMode == MOVE_Custom) && (CustomMovementMode == MOVE_Swing) && UpdatedComponent;
	// return isSwinging;
}
void USSDPlayerMovementComponent::BackDash(){

}
void USSDPlayerMovementComponent::KnockBack(const FHitResult& Hit){
	FVector knock = KnockBackVelocity;
	if (FMath::IsNearlyZero(Hit.ImpactNormal.Y, 0.2f)) { knock.Y *= CharacterOwner->GetActorForwardVector().Y;	}
	else { knock.Y *= -1 * Hit.ImpactNormal.Y; }

	Velocity = knock;
	SetMovementMode(MOVE_Falling);
}


void USSDPlayerMovementComponent::JumpOffWall() {
	SetMovementMode(MOVE_Falling);
	Velocity = FVector::ZeroVector;
	Velocity = jumpWallVelocity;
	Velocity.Y *= -1*towardWall;

}
bool USSDPlayerMovementComponent::IsUpdrafting() const {
	return bUpdraft;
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
void USSDPlayerMovementComponent::LogAtReducedRate(FString Log, int ratePerFrames) {
	frameCount++;
	if (frameCount % ratePerFrames == 0) {
		UE_LOG(LogCharacterMovement, Log, TEXT("%s"), *Log);
		frameCount = 1;
	}
}
