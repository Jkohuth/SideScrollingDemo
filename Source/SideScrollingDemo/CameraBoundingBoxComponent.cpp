// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraBoundingBoxComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Engine.h"

// Sets default values for this component's properties
UCameraBoundingBoxComponent::UCameraBoundingBoxComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	BoundingBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Bounds"));
	BoundingBox->InitBoxExtent(BoxSize);
	BoundingBox->SetRelativeLocation(FVector(0.f, 0.f,0.f));
	BoundingBox->bVisible = true;
	BoundingBox->bHiddenInGame = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	CameraComponent->SetupAttachment(BoundingBox);
}


// Called when the game starts
void UCameraBoundingBoxComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
	
}


// Called every frame
void UCameraBoundingBoxComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCameraBoundingBoxComponent::UpdatePosition(UCapsuleComponent* targetCapsule){
	if (targetCapsule == NULL) {
		return;
	}
	TargetLocation = targetCapsule->GetComponentLocation();
	halfHeight = targetCapsule->GetUnscaledCapsuleHalfHeight();
	radius = targetCapsule->GetUnscaledCapsuleRadius();


	targetLeft = TargetLocation.Y + radius;
	targetRight = TargetLocation.Y - radius;
	targetTop = TargetLocation.Z + halfHeight;
	targetBottom = TargetLocation.Z - halfHeight;

	//Origin = this->GetActorLocation();
	Origin  = BoundingBox->GetComponentLocation();
	BoxExtent = BoundingBox->GetUnscaledBoxExtent();
	
	right = Origin.Y - BoxExtent.Y; // The sign has to do with the way the screen is oriented
	left = Origin.Y + BoxExtent.Y;
	top = Origin.Z + BoxExtent.Z;
	bottom = Origin.Z - BoxExtent.Z;




	float shiftY = 0;
	float shiftZ = 0;

	// Has to do with dumb orientation I need to fix that
	if (targetRight < right) {
		shiftY = targetRight - right;
	}
	else if (targetLeft > left) {
		shiftY = targetLeft - left;
	}
	left += shiftY;
	right += shiftY;

	if (targetBottom < bottom) {
		shiftZ = targetBottom - bottom;
	}
	else if (targetTop > top) {
		shiftZ = targetTop - top;
	}

	//this->SetActorLocation(FVector(Origin.X, Origin.Y + shiftY, Origin.Z + shiftZ));
	BoundingBox->SetWorldLocation(FVector(Origin.X, Origin.Y + shiftY, Origin.Z + shiftZ));
}
void UCameraBoundingBoxComponent::InitializePosition(UCapsuleComponent* targetCapsule) {
	BoundingBox->SetWorldLocation(targetCapsule->GetComponentLocation());
}
void UCameraBoundingBoxComponent::ResetCamera(AActor* targetActor) {

	TargetLocation = targetActor->GetActorLocation();
	halfHeight = targetActor->GetSimpleCollisionHalfHeight();
	radius = targetActor->GetSimpleCollisionRadius();
	
	FVector OffsetGround = TargetLocation;
	OffsetGround.Z += (BoxExtent.Z - halfHeight);

	BoundingBox->SetWorldLocation(OffsetGround);	
}