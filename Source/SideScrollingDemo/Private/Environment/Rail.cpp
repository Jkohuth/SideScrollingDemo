// Fill out your copyright notice in the Description page of Project Settings.

#include "Rail.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "CameraBoundingBox.h"
#include "Engine.h"
#include "Components/SplineMeshComponent.h"
#include "MalePlayer.h"
#include "malePlayerMovementComponent.h"
#include "Components/CapsuleComponent.h"


// Sets default values
ARail::ARail()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//CylinderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rail Mesh"));
	RailSpline = CreateDefaultSubobject<USplineComponent>(TEXT("Rail Spline"));
	CylinderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cylinder Mesh"));
	//RailCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Rail Capsule"));

	RootComponent = RailSpline;
	//RailCapsule->SetupAttachment(RootComponent);
	CylinderMesh->SetupAttachment(RootComponent);
	//RailSpline->SetWorldLocation(RailCapsule->GetComponentLocation());
	//RailCapsule->AttachTo(RootComponent);
	//RailCapsule->bHiddenInGame = false;
	//RailSpline->SetupAttachment(RootComponent);
	//RailCapsule->SetCapsuleHalfHeight(splineLength);
	RailSpline->SetDrawDebug(true); // Draw this!!

	heightOfRays = 10.0f;
	DistanceBetweenLines = 15.0f;
}

// Called when the game starts or when spawned
void ARail::BeginPlay()
{
	Super::BeginPlay();

	splineLength = RailSpline->GetSplineLength();
	numberOfLines = splineLength / DistanceBetweenLines;

	UE_LOG(LogClass, Log, TEXT("The spline length on begin play %s"), *FString::SanitizeFloat(splineLength));
	
	//RailCapsule->SetCapsuleHalfHeight(splineLength);
	//RailCapsule->SetCapsuleHalfHeight(splineLength/2);
	//float capsuleHalfHeight = RailCapsule->GetScaledCapsuleHalfHeight();
	//UE_LOG(LogClass, Log, TEXT("Scaled Capsule Half Height %s"), *FString::SanitizeFloat(capsuleHalfHeight));

	//RailCapsule->SetCapsuleHalfHeight(splineLength);
	FVector halfWayPoint = RailSpline->GetLocationAtDistanceAlongSpline(splineLength / 2, ESplineCoordinateSpace::World);
	//RailCapsule->SetWorldLocation(halfWayPoint);
	//FString tmp = FString::SanitizeFloat(splineLength);
	//UE_LOG(LogClass, Log, TEXT("The length of the spline %s"), *tmp);
	//    UE_LOG(LogClass, Log, TEXT("The rails location %s"), *GetActorLocation().ToCompactString());

}

// Called every frame
void ARail::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//RailRayCast();
}

void ARail::RailRayCast() {
	// from begining to end on the spline find the +Z direction and draw a raycast across a length up for the duration

	FName TraceTag("RailTag");
	TArray<FHitResult> HitResults;
	
	//GetWorld()->DebugDrawTraceTag = TraceTag;

	FVector Start = GetActorLocation();
	Start.Z += 5.f;
	//UE_LOG(LogClass, Log, TEXT("%s"), *Start.ToCompactString());
	//Start.Y = Start.Y - length / 2;
	FRotator rotate = GetActorRotation();
	//UE_LOG(LogClass, Log, TEXT("The Rotation of the actor %s"), *rotate.ToCompactString());
	//FVector End = FVector(Start.X, Start.Y, (Start.Z + heightOfRays));
	
	//heightAdd = (splineLength * FMath::Cos(-40.f)) / numberOfLines;
	//lengthAdd = (splineLength * FMath::Sin(-40.f)) / numberOfLines;
	lengthAdd = (splineLength) / numberOfLines;

	//FCollisionShape LineShape = FCollisionShape::LineShape;
	//LineShape = FCollisionShape::;

	FCollisionQueryParams CollisionParams(FName(TEXT("Rail")), true, this);
	CollisionParams.TraceTag = TraceTag;
	CollisionParams.AddIgnoredActor(this);

	Start = RailSpline->GetWorldLocationAtDistanceAlongSpline(0.f);
	Start.Z += 5.0f;
	FVector End = FVector(Start.X, Start.Y, (Start.Z + heightOfRays));
	for (int i = 0; i < numberOfLines; i++) {
		
		Start = RailSpline->GetWorldLocationAtDistanceAlongSpline(lengthAdd*i);
		Start.Z += 5.0f;
		End = FVector(Start.X, Start.Y, (Start.Z + heightOfRays));

		DrawDebugLine(GetWorld(), Start, End, FColor::Red, true, 1.f);
		if (GetWorld()->LineTraceMultiByChannel(HitResults, Start, End, ECollisionChannel::ECC_WorldDynamic, CollisionParams)) {
			if (HitResults.Num() > 0) {
				int arrayIndex = -1;
				TArray<FHitResult>::TConstIterator ActorIt = HitResults.CreateConstIterator();
				for (; ActorIt; ++ActorIt) {
					arrayIndex++;
					//DrawDebugLine(GetWorld(), Start, End, FColor::Blue, true, .5f);
					//UE_LOG(LogClass, Log, TEXT("But her aim is getting better %s"), *ActorIt->GetActor()->GetDebugName());
					AMalePlayer* Player = Cast<AMalePlayer>(ActorIt->GetActor());
					if (Player) {
						if ( (Player->GetMalePlayerMovement()->MovementMode == MOVE_Custom && 
								Player->GetMalePlayerMovement()->CustomMovementMode == ECustomMovementMode::MOVE_Rail) 
							|| Player->GetMalePlayerMovement()->isJumping) {
							return; // Do nothing we are already on a rail or are trying to jump off it
						}
						//else if (Player->GetMalePlayerMovement()->MovementMode == MOVE_Falling && Player->bPressedJump == true) return;
						DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, .5f);
						//FVector Angle = RailSpline->GetDirectionAtDistanceAlongSpline(lengthAdd*i, ESplineCoordinateSpace::World);
						Player->GetMalePlayerMovement()->AttachToRail(GetRailSpline());
					}
				}
			}
		}
		//tart.Y += lengthAdd;
		//End.Y += lengthAdd;
		//Start.Z += heightAdd; // The angle is negative
		//End.Z += heightAdd; // The angle is negative
		DistanceBetweenLines -= DistanceBetweenLines;


	}
	FHitResult result(1.0f);
	//return false;
}
FVector ARail::GetSplineLocation() {
	return SplineLocation;
}