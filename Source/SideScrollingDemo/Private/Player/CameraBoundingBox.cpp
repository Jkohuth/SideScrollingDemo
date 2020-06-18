// Fill out your copyright notice in the Description page of Project Settings.

#include "CameraBoundingBox.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Engine.h"

// Sets default values
ACameraBoundingBox::ACameraBoundingBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BoundingBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Bounds"));
	RootComponent = BoundingBox;
	BoundingBox->InitBoxExtent(BoxSize);
	BoundingBox->SetRelativeLocation(FVector(0.f, 0.f,0.f));
	BoundingBox->SetVisibility(true);


	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	CameraComponent->SetupAttachment(RootComponent);
	CameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	CameraComponent->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	CameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
}

// Called when the game starts or when spawned
void ACameraBoundingBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACameraBoundingBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACameraBoundingBox::UpdatePosition(AActor* targetActor) {
	if (targetActor == NULL) {
		return;
	}
	TargetLocation = targetActor->GetActorLocation();
	halfHeight = targetActor->GetSimpleCollisionHalfHeight();
	radius = targetActor->GetSimpleCollisionRadius();


	targetLeft = TargetLocation.Y + radius;
	targetRight = TargetLocation.Y - radius;
	targetTop = TargetLocation.Z + halfHeight;
	targetBottom = TargetLocation.Z - halfHeight;

	Origin = this->GetActorLocation();
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

	this->SetActorLocation(FVector(Origin.X, Origin.Y + shiftY, Origin.Z + shiftZ));


}
void ACameraBoundingBox::InitializePosition(AActor* targetActor) {
	BoundingBox->SetWorldLocation(targetActor->GetActorLocation());
}
void ACameraBoundingBox::ResetCamera(AActor* targetActor) {

	TargetLocation = targetActor->GetActorLocation();
	halfHeight = targetActor->GetSimpleCollisionHalfHeight();
	radius = targetActor->GetSimpleCollisionRadius();
	
	FVector OffsetGround = TargetLocation;
	OffsetGround.Z += (BoxExtent.Z - halfHeight);

	this->SetActorLocation(OffsetGround);
	
}