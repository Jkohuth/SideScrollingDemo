// Fill out your copyright notice in the Description page of Project Settings.
 
#include "MalePlayerMovementComponent.h"
#include "Constants.h"
#include "Engine/World.h"
#include "Engine.h"
#include "Engine/EngineTypes.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "Components/PrimitiveComponent.h"
//#include "GameFramework/MovementComponent.h"
#include "GameFramework/PhysicsVolume.h"
#include "CameraBoundingBox.h"
#include "Components/SplineComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogCharacterMovement, Log, All);

void UMalePlayerMovementComponent::InitializeComponent() {
	Super::InitializeComponent();

	bConstrainToPlane = true;
	SetPlaneConstraintAxisSetting(LockXAxis);
	bOrientRotationToMovement = true;
	//GravityScale = Gravity;
	AirControl = 0.60f;
	//AirControl = 1.0f;
	JumpZVelocity = JumpVelocity;
	GroundFriction = 3.f;
	MaxWalkSpeed = 1300.f;
	MaxFlySpeed = 600.f;
	GravityScale = RisingGravityScalar;
	bNotifyApex = true;
	MaxAcceleration = 3500.0f;
	WallSlideFriction = 30.0f;

	MaxMovementSpeeds = FVector(0.f, MaxWalkSpeed, JumpZVelocity);
	RailSpeed = FVector(0.f, -500.0f, 0.f);
}
void UMalePlayerMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
void UMalePlayerMovementComponent::BeginPlay() {
	Super::BeginPlay();
	if (CharacterOwner) {
		capsuleHalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		capsuleRadius = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();
		wallLinesCount = 10.0f; // 30 Lines will be drawn
		wallLinesSpace = (2 * capsuleHalfHeight )/ wallLinesCount;
		//wallRayCheck.Init(false, wallLinesCount);
	}
}

void UMalePlayerMovementComponent::OnJumpInput() {
	//if (CheckCustomMovementMode(ECustomMovementMode::MOVE_Rail)) {
	//	this->SetMovementMode(EMovementMode::MOVE_Falling);
	//}
	if (CheckCustomMovementMode(ECustomMovementMode::MOVE_Wall)) {
		JumpOffWall();
	}
	else if (CheckCustomMovementMode(ECustomMovementMode::MOVE_Rail)) {
		bJumpOffRail = true;
	}
}
void UMalePlayerMovementComponent::SetCustomMovementMode(uint8 custom) {
	SetMovementMode(MOVE_Custom, custom);
}

void UMalePlayerMovementComponent::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) {

	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	switch (PrevMovementMode) {
	case EMovementMode::MOVE_Custom:
		switch (PreviousCustomMode) {
		case ECustomMovementMode::MOVE_Wall:
			bSlidingDownWall = false;
			break;
		case ECustomMovementMode::MOVE_Rail:
		//	UE_LOG(LogClass, Log, TEXT("This just changed and I'm trying to figure out why"));
		//	if(MovementMode == MOVE_Falling)
		//		UE_LOG(LogClass, Log, TEXT("It went to falling for some rasin"));
			//RailSplineReference = NULL;
			break;		
		}
		break;
	case EMovementMode::MOVE_Falling:
		SetGravity(RisingGravityScalar);
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
void UMalePlayerMovementComponent::PhysCustom(float DeltaTime, int32 Iterations) {
	switch (CustomMovementMode) {
	case MOVE_Wall:
		PhysWall(DeltaTime, Iterations);
		break;
	case MOVE_Rail:
		PhysRail(DeltaTime, Iterations);
		break;
	case MOVE_Path:
		PhysPath(DeltaTime, Iterations);
		break;
	default:
		UE_LOG(LogCharacterMovement, Warning, TEXT("Unsupported Movement Mode %d"), int32(MovementMode));
		break;
	}

}
void UMalePlayerMovementComponent::PhysWall(float DeltaTime, int32 Iterations) {
	if (DeltaTime < MIN_TICK_TIME) return;

	// Make sure the character own is valid to move if not stop it from moving and return
	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->Role != ROLE_SimulatedProxy))) {
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}
	if (!UpdatedComponent->IsQueryCollisionEnabled()) { SetMovementMode(MOVE_Custom, ECustomMovementMode::MOVE_Wall); return; }


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
		Acceleration.X = 0.f;
		Acceleration.Y = 0.f;

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
		End.Y += (-1*wallDirection.Y * (capsuleRadius + 10.f));
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(CharacterOwner);
		bool isHit = GetWorld()->LineTraceSingleByChannel(WallHit, Start, End, ECollisionChannel::ECC_WorldStatic, CollisionParams);
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, 1.f);
		if (!isHit) {
			SetMovementMode(MOVE_Falling);
			StartNewPhysics(remainingTime + timeTick, Iterations - 1);
			return;
		}
	}
}
// This code heavily borrows from the CharacterMovementComponent.cpp:PhysWalking() source... Ideally that means this should be the correct way to implement it
void UMalePlayerMovementComponent::PhysRail(float DeltaTime, int32 Iterations) {

	if (DeltaTime < MIN_TICK_TIME) return;
	if (!RailSplineReference->IsValidLowLevel()) return;
	// Make sure the character own is valid to move if not stop it from moving and return
	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->Role != ROLE_SimulatedProxy))) {
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}
	if (!UpdatedComponent->IsQueryCollisionEnabled()) { SetMovementMode(MOVE_Custom, ECustomMovementMode::MOVE_Rail); return; }

	float remainingTime = DeltaTime;
	
	while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity() || (CharacterOwner->Role == ROLE_SimulatedProxy)))
	{
		Iterations++;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;

		// Store Current Values
		UPrimitiveComponent *const OldBase = GetMovementBase();
		const FVector PreviousBaseLocation = (OldBase != NULL) ? OldBase->GetComponentLocation() : FVector::ZeroVector;
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();

		RestorePreAdditiveRootMotionVelocity();

		FVector OldVelocity = Velocity;
		
		FVector Forward = CharacterOwner->GetActorForwardVector();

		FVector RailSplineDirection = RailSplineReference->GetWorldDirectionAtDistanceAlongSpline(distanceAlongSpline);
		float GravityDir = FVector::DotProduct(FVector(0.f, 0.f, GetGravityZ()), RailSplineDirection);
		FVector GravityAccel = GravityDir * RailSplineDirection;
		GravityAccel = FVector(0.f, GravityAccel.Y, GravityAccel.Z);
		//UE_LOG(LogClass, Log, TEXT("GravityAccel %s | RailAcceleration %s"), *GravityAccel.ToCompactString(), *RailAcceleration.ToString());

		//FMath::Clamp(splineSpeed, -MaxSplineSpeed, MaxSplineSpeed);
		if (GravityAccel.Y == 0.f) {
			GravityAccel.Y = Forward.Y * 20.0f;
		}
		Velocity += GravityAccel * timeTick;

		//Apply Gravity
		FRotator splineRotater = RailSplineReference->FindRotationClosestToWorldLocation(GetActorFeetLocation(), ESplineCoordinateSpace::World);

		FHitResult Hit(1.f);
		FVector Adjusted = 0.5f*(Velocity + OldVelocity) * timeTick;
		distanceAlongSpline += Adjusted.SizeSquared() * timeTick;
		//UE_LOG(LogClass, Log, TEXT("Adjusted %s"), *Adjusted.ToString());
		
		SafeMoveUpdatedComponent(Adjusted, UpdatedComponent->GetComponentQuat(), true, Hit);
		if (Hit.IsValidBlockingHit()) {
			FVector location = GetActorLocation();
			location.Z += 5.f;
			CharacterOwner->SetActorLocation(location);
			// I am in shock that this crude and simple fix worked so well
		}
		
		FHitResult RailHit;
		FVector Start = GetActorFeetLocation();
		FVector End = Start;
		float tolerence = -20.f;
		End.Z +=  tolerence;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(CharacterOwner);
		
		bool isHit = GetWorld()->LineTraceSingleByChannel(RailHit, Start, End, ECollisionChannel::ECC_WorldStatic, CollisionParams);
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, 1.f);
		if (isHit && RailHit.GetActor() && !RailHit.GetActor()->ActorHasTag(FName("rail"))) {
			SetMovementMode(MOVE_Walking);
			StartNewPhysics(remainingTime + timeTick, Iterations - 1);
			return;
		}

		RailSplineDirection = RailSplineReference->GetWorldDirectionAtDistanceAlongSpline(distanceAlongSpline);
		if (bJumpOffRail) {
			bJumpOffRail = false; 
			SetMovementMode(MOVE_Falling);
			
			// Clamp max Jump Velocity
			FVector JumpOffRail = JumpVelocity * FVector(0.f, FMath::Sin(FMath::DegreesToRadians(splineRotater.Pitch)), FMath::Cos(FMath::DegreesToRadians(splineRotater.Pitch)));
			Velocity += JumpOffRail;
			UE_LOG(LogClass, Log, TEXT("JumpOffRail %s"), *JumpOffRail.ToCompactString());

			StartNewPhysics(remainingTime + timeTick, Iterations - 1);

		}
	}
}
void UMalePlayerMovementComponent::PhysPath(float DeltaTime, int32 Iterations) {

}
void UMalePlayerMovementComponent::AttachToPath(USplineComponent* PathSpline) {
	FVector LocationClosets = PathSpline->FindLocationClosestToWorldLocation(GetActorLocation(), ESplineCoordinateSpace::World);
	UE_LOG(LogCharacterMovement, Log, TEXT("LocationClosest %s"), *LocationClosets.ToCompactString());

	FVector SplineBeginning = PathSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
	FVector SplineEnding = PathSpline->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::World);

	FVector splinePoint1 = SplineBeginning - LocationClosets;
	FVector splinePoint2 = SplineEnding - LocationClosets;
	SetMovementMode(MOVE_Custom, ECustomMovementMode::MOVE_Path);
	PathSplineRef = PathSpline;
	Velocity = FVector::ZeroVector;
	if (splinePoint1.SizeSquared() < splinePoint2.SizeSquared()) {
		CharacterOwner->SetActorLocation(SplineBeginning);
	}
	else {
		CharacterOwner->SetActorLocation(SplineEnding);

	}


}
void UMalePlayerMovementComponent::ConfirmPathExit() {

}
void UMalePlayerMovementComponent::MoveRightInput(float Value) {
	// This will handle all input and call all necessary functions with CustomMovementComponent;;
	//if (CheckCustomMovementMode(ECustomMovementMode::MOVE_Rail)) {
	//	AddInputVector(FVector(0.0f, -Value, 0.0f));
	//}
	//else {
	if (CheckCustomMovementMode(ECustomMovementMode::MOVE_Path)) {

	}else{
		AddInputVector(FVector(0.0f, -Value, 0.0f));
	}
}
void UMalePlayerMovementComponent::MoveUpInput(float Value) {
	if (CheckCustomMovementMode(ECustomMovementMode::MOVE_Wall)) {
		if(Value == -1.f)
			AddInputVector(FVector(0.f, 0.f, Value));
	}
}

void UMalePlayerMovementComponent::SetGravity(float NewGravity) {
	GravityScale = NewGravity;
}
void UMalePlayerMovementComponent::TriggerWallMovement(FHitResult Hit) {

		// If ImpactPoint.Y = -1 the wall is on the RightHandSide
	FName TraceTag("WallTag");
	FHitResult HitResult;

	FVector StartTrace = GetActorLocation();
	FVector EndTrace = StartTrace;
	EndTrace.Y += -1.f*100.0f*Hit.ImpactNormal.Y; // Direction the wall is facing, must be negative because the level is set facing negative
	EndTrace.Z += capsuleHalfHeight;
	StartTrace.Z += capsuleHalfHeight;
	wallDirection.Y = Hit.ImpactNormal.Y; // Perhaps something needs to be done about the variable wallDirection but I worry about that later

	FCollisionQueryParams CollisionParams;
	//CollisionParams.TraceTag = TraceTag;
	CollisionParams.AddIgnoredActor(CharacterOwner);
	//CollisionParams.AddIgnoredActor(); Find out how to remove boundingbox
	for (int i = 0; i < wallLinesCount; i++) {
		//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, 1.f);

		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECollisionChannel::ECC_WorldStatic, CollisionParams)) {
			wallRayCheck.Emplace(true);
			// I calculate the rays from the top down
			// if All of the rays hit the same object then stick to wall
			// if the top 20% of rays don't hit an object but the rest do then grab the ledge
			// if the bottom 50% of rays hit a wall but the top 50% dont then roll onto ledge
			// Perhaps an array that can hold all the HitResults of every iteration
			// I want something like if(GetWorld()->LineTraceSingleByChannel()
			// if(HitResult && wallLinesCount/5 <- but it has to be false for all of them
		}
		else wallRayCheck.Emplace(false);
		//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, true, 1.0f);
		EndTrace.Z -= wallLinesSpace;
		StartTrace.Z -= wallLinesSpace;
	}
	WallCollisionHandler();
}
void UMalePlayerMovementComponent::WallCollisionHandler() {
	// We need a top down approach for calculate
	// This isn't the best way to calculate the roll onto ledge but I'll cross that bridge later
	float truePercent = 0.f;
	float falsePercent = 0.f;
	int midpoint = wallLinesCount / 2;
	//float amount = 1 / wallLinesCount;

	for (int i = 0; i < wallLinesCount; i++) {
		//if (wallRayCheck[i]) truePercent += amount;
		//else falsePercent += amount;
		wallRayCheck[i] ? (truePercent++) : (falsePercent++);
		wallRayCheck[i] = false; // have to reset it after getting the data we want a
	}
	truePercent = truePercent / wallLinesCount;
	falsePercent = falsePercent / wallLinesCount;

	if (truePercent >= 0.6f) {
		Velocity = FVector::ZeroVector;
  		SetCustomMovementMode(ECustomMovementMode::MOVE_Wall);
	}
	else if (!wallRayCheck[0] && !wallRayCheck[midpoint]) { // if half of the rays return false including the top and the midpoint  
		//Do roll onto ledge
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "You're trying to roll");

		//CharacterOwner->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		FVector updatedLocation = GetActorLocation();

		UE_LOG(LogClass, Log, TEXT("The updated location %s"), *updatedLocation.ToCompactString());
		updatedLocation.Y -= (wallDirection.Y*capsuleRadius*2);
		updatedLocation.Z += capsuleHalfHeight;
		FString tmp = FString::SanitizeFloat(wallDirection.Y);
		UE_LOG(LogClass, Log, TEXT("The walldirection %s"), *tmp);
		tmp = FString::SanitizeFloat(capsuleRadius);
		UE_LOG(LogClass, Log, TEXT("The radisu %s"), *tmp);

		UE_LOG(LogClass, Log, TEXT("The updated location %s"), *updatedLocation.ToCompactString());
		CharacterOwner->SetActorLocation(updatedLocation);


	}
	wallRayCheck.Reset(0);

}
void UMalePlayerMovementComponent::CheckWallBehind()
{
	if (MovementMode != EMovementMode::MOVE_Falling || MovementMode != EMovementMode::MOVE_Custom) return; //Dont get stuck to the wall on the ground
	if (CheckCustomMovementMode(ECustomMovementMode::MOVE_Wall)) {

	}
	WallCheckLocationStart = GetActorLocation();
	WallCheckLocationEnd = FVector(WallCheckLocationStart.X, WallCheckLocationStart.Y + (-1*FacingDirection.Y), WallCheckLocationStart.Z);
	FHitResult outHit;
	bool isHit = GetWorld()->LineTraceSingleByChannel(outHit, WallCheckLocationStart, WallCheckLocationEnd, ECC_Visibility);
	DrawDebugLine(GetWorld(), WallCheckLocationStart, WallCheckLocationEnd, FColor::Red);
	if (isHit) {
		UE_LOG(LogClass, Log, TEXT("YUUUUUUUUGGGGGEEEE WALL"));
		Velocity = FVector::ZeroVector;
		SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::MOVE_Wall);
	}
}

void UMalePlayerMovementComponent::FallOffWall(float Value)
{
	if (CheckCustomMovementMode(ECustomMovementMode::MOVE_Wall) && Value == -wallDirection.Y) {
		wallDirection = FVector::ZeroVector;
		SetMovementMode(MOVE_Falling);
		
	}

}

void UMalePlayerMovementComponent::InitiateSlide(float DeltaTime)
{
	FHitResult GroundHitCheck = FHitResult();
	FVector NewActorLocation = GetActorLocation();
	NewActorLocation.Z = NewActorLocation.Z + (wallSlideVelocity.Z * DeltaTime);

	//SetActorLocation(NewActorLocation, true, &GroundHitCheck);
	WallCheckLocationStart = GetActorLocation();
}

void UMalePlayerMovementComponent::JumpOffWall()
{
	SetMovementMode(MOVE_Falling);
	Velocity = FVector::ZeroVector;
	Velocity = FVector(wallJumpVelocity.X, wallDirection.Y * wallJumpVelocity.Y, wallJumpVelocity.Z);

}
bool UMalePlayerMovementComponent::CheckCustomMovementMode(uint8 CustomMode)
{
	if (MovementMode == MOVE_Custom && CustomMovementMode == CustomMode) return true;

	return false;
}
void UMalePlayerMovementComponent::SetFacingDirection(FVector Value) {
	FacingDirection = Value;
}



void UMalePlayerMovementComponent::AttachToRail(USplineComponent* RailSpline) {
	if (MovementMode == MOVE_Custom && CustomMovementMode == MOVE_Rail) {
		return; // Do nothing we are already on a rail
	}
	else {
		SetMovementMode(MOVE_Custom, ECustomMovementMode::MOVE_Rail);
		RailSplineReference = RailSpline;
		FVector LocationClosest = RailSplineReference->FindLocationClosestToWorldLocation(GetActorFeetLocation(), ESplineCoordinateSpace::World);
		FVector OldVelocity = Velocity;
		Velocity = FVector::ZeroVector;
		for (int32 i = 0; i <= RailSplineReference->GetNumberOfSplinePoints(); i++) {
			int32 j = i + 1;
			if (RailSplineReference->GetNumberOfSplinePoints() < j) {
				// Sanity Check
				return;
			}
			FVector splinePoint1 = RailSplineReference->GetWorldLocationAtSplinePoint(i);
			FVector splinePoint2 = RailSplineReference->GetWorldLocationAtSplinePoint(j);
			FVector beforeSpline = RailSplineReference->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);

			if (GetActorFeetLocation().Y > beforeSpline.Y) {
				beforeSpline.Z += capsuleHalfHeight;
				beforeSpline.Y -= 2*capsuleRadius;
				distanceAlongSpline = RailSplineReference->GetDistanceAlongSplineAtSplinePoint(0);
				if (distanceAlongSpline == 0.0f) {
					distanceAlongSpline = 1.0f;
				}
				FVector worldDirectionAtDis = RailSplineReference->GetWorldDirectionAtDistanceAlongSpline(distanceAlongSpline);

				CharacterOwner->SetActorLocation(beforeSpline);
				//UE_LOG(LogCharacterMovement, Log, TEXT("Velocity before %s"), *Velocity.ToCompactString());
				//Velocity = FVector(Velocity.X * worldDirectionAtDis.X, Velocity.Y * worldDirectionAtDis.Y, Velocity.Z * worldDirectionAtDis.Z);
				//UE_LOG(LogCharacterMovement, Log, TEXT("Velocity after %s"), *Velocity.ToCompactString());
				splineSpeed = FVector::DotProduct(OldVelocity, worldDirectionAtDis);
				Velocity = splineSpeed * worldDirectionAtDis;
				return;
			}
			// Make sure to add Z when you tilt the damn thing
			else if (splinePoint1.Y <= GetActorFeetLocation().Y && GetActorFeetLocation().Y <= splinePoint2.Y ||
				splinePoint2.Y <= GetActorFeetLocation().Y && GetActorFeetLocation().Y <= splinePoint1.Y) {

				float DisFromPoint1ToActor = DistanceBetweenTwoVectors(GetActorFeetLocation(), splinePoint1);
				float DisFromPoint1ToSplinePoint2 = DistanceBetweenTwoVectors(splinePoint2, splinePoint1);
				distanceAlongSpline = RailSplineReference->GetDistanceAlongSplineAtSplinePoint(i);
				distanceAlongSpline += DisFromPoint1ToActor;
				
				FVector worldLocationDisAlongSpline = RailSplineReference->GetLocationAtDistanceAlongSpline(distanceAlongSpline, ESplineCoordinateSpace::World);
				FVector worldDirectionAtDis = RailSplineReference->GetWorldDirectionAtDistanceAlongSpline(distanceAlongSpline);
				
				worldLocationDisAlongSpline.Z += capsuleHalfHeight;
				CharacterOwner->SetActorLocation(worldLocationDisAlongSpline);
				
				// Direction is a unit vector in the direction of the spline, so use Dot Product to find Default Spline Speed
				// Multiply velocity by direction unit vector because we also need the components
				splineSpeed = FVector::DotProduct(OldVelocity, worldDirectionAtDis);

				Velocity = splineSpeed * worldDirectionAtDis;
				return; // Already got what we needed
			}
		}
	}
}
float UMalePlayerMovementComponent::DistanceBetweenTwoVectors(FVector one, FVector two) {
	float x = FMath::Square(one.X - two.X);
	float y = FMath::Square(one.Y - two.Y);
	float z = FMath::Square(one.Z - two.Z);

	return FMath::Sqrt(x + y + z);
}

FVector UMalePlayerMovementComponent::AverageBetweenSplinePoints(FVector splinePoint1, FVector splinePoint2) {

	FVector Result;
	Result = GetActorFeetLocation() - splinePoint1;
	
	FVector divsor = (splinePoint2 - splinePoint1);
	if (divsor.Z < KINDA_SMALL_NUMBER) {
		divsor.Z = 1;
	}
	Result = Result / divsor;

	UE_LOG(LogCharacterMovement, Log, TEXT("The average between 2 spline points %s | %s"), *Result.ToCompactString(), *divsor.ToCompactString());
	return Result;


}
void UMalePlayerMovementComponent::AccumulateForce(FVector Force) {
	this->Velocity += Force;
}