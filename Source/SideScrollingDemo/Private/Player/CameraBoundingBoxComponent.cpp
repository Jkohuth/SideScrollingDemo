// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraBoundingBoxComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "SideScrollingDemoTypes.h"
#include "LevelCameraFollowBounds.h"

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
	// Have a Camera Transform in the blueprints its not necessary
	//CameraComponent->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
	BoundingBox->OnComponentBeginOverlap.AddDynamic(this, &UCameraBoundingBoxComponent::OnBoundingBoxOverlapBegin);
	BoundingBox->OnComponentEndOverlap.AddDynamic(this, &UCameraBoundingBoxComponent::OnBoundingBoxOverlapEnd);
	BoundingBox->OnComponentHit.AddDynamic(this, &UCameraBoundingBoxComponent::OnHit);

	BoxExtent = BoundingBox->GetUnscaledBoxExtent();
}

void UCameraBoundingBoxComponent::InitializeComponent() {
	Super::InitializeComponent();

}
// Called when the game starts
void UCameraBoundingBoxComponent::BeginPlay()
{
	Super::BeginPlay();
	Origin = BoundingBox->GetComponentLocation();
	// ...
	
}


// Called every frame
void UCameraBoundingBoxComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
void UCameraBoundingBoxComponent::OnBoundingBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Everytime this is called");

	// Maybe using tags for everything isn't idea but before I start effecting performance you need to have something made
	if (OtherComp && OtherComp->ComponentHasTag(ECustomTags::LevelBoundsTag)) {
		UBoxComponent* NewBounds = Cast<UBoxComponent>(OtherComp);
		ALevelCameraFollowBounds* levelBounds = Cast<ALevelCameraFollowBounds>(OtherComp->GetAttachmentRootActor());
		if (NewBounds && levelBounds) {
			if (CameraFollowLocation == FVector::ZeroVector) {
				CameraFollowExtents = NewBounds->GetScaledBoxExtent();
				CameraFollowLocation = levelBounds->GetActorLocation() + NewBounds->GetRelativeTransform().GetLocation();
			}
			else {
				CameraFollowLocationPrevious = CameraFollowLocation;
				CameraFollowExtentsPrevious = CameraFollowExtents;
				CameraFollowLocationNext = levelBounds->GetActorLocation() + NewBounds->GetRelativeTransform().GetLocation();
				CameraFollowExtentsNext = NewBounds->GetScaledBoxExtent();

				// Will have to clean this up later
				CameraFollowExtents += NewBounds->GetScaledBoxExtent();
			}
		}
	}
}
void UCameraBoundingBoxComponent::OnBoundingBoxOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

	if (OtherComp && OtherComp->ComponentHasTag(ECustomTags::LevelBoundsTag)) {
		UBoxComponent* OldBounds = Cast<UBoxComponent>(OtherComp);
		ALevelCameraFollowBounds* levelBounds = Cast<ALevelCameraFollowBounds>(OtherComp->GetAttachmentRootActor());
		if (OldBounds && levelBounds) {
			// Here we left the one we don't care about anymore
			FVector previousCameraFollowLocation = levelBounds->GetActorLocation() + OldBounds->GetRelativeTransform().GetLocation();
			// We left the old one so update the extents
			if (previousCameraFollowLocation == CameraFollowLocationPrevious) {
				CameraFollowLocation = CameraFollowLocationNext;
				CameraFollowExtents = CameraFollowExtentsNext;
			}
			else if (previousCameraFollowLocation == CameraFollowLocationNext) {
				CameraFollowLocation = CameraFollowLocationPrevious;
				CameraFollowExtents = CameraFollowExtentsPrevious;
			}
		}
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Overlap End has been called");
		
	}
}
void UCameraBoundingBoxComponent::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Hit the level bounds");

	if (OtherComp && OtherComp->ComponentHasTag(ECustomTags::LevelBoundsTag)) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Hit the level bounds");

	}
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
	//BoxExtent = BoundingBox->GetUnscaledBoxExtent();
	
	right = Origin.Y - BoxExtent.Y; // The sign has to do with the way the screen is oriented
	left = Origin.Y + BoxExtent.Y;
	top = Origin.Z + BoxExtent.Z;
	bottom = Origin.Z - BoxExtent.Z;

	//FString tmpBounds = CameraFollowLocation.ToCompactString() + " " + CameraFollowExtents.ToCompactString();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, tmpBounds);
	
	float levelBoundsRight = CameraFollowLocation.Y - CameraFollowExtents.Y;
	float levelBoundsLeft = CameraFollowLocation.Y + CameraFollowExtents.Y;
	float levelBoundsTop = CameraFollowLocation.Z + CameraFollowExtents.Z;
	float levelBoundsBottom = CameraFollowLocation.Z - CameraFollowExtents.Z;



	float shiftY = 0;
	float shiftZ = 0;
	shift.Y = 0;
	shift.Z = 0;

	// Has to do with dumb orientation I need to fix that
	if (targetRight > levelBoundsRight && targetRight < right) {
		shift.Y = targetRight - right;
	}
	else if (targetLeft < levelBoundsLeft &&targetLeft > left) {
		shift.Y = targetLeft - left;
	}
	left += shift.Y;
	right += shift.Y;

	if (targetBottom > levelBoundsBottom && targetBottom < bottom) {
		shift.Z = targetBottom - bottom;
	}
	else if (targetTop < levelBoundsTop && targetTop > top) {
		shift.Z = targetTop - top;
	}

	//this->SetActorLocation(FVector(Origin.X, Origin.Y + shiftY, Origin.Z + shiftZ));
	FVector updatedLocation = FVector(0.f, Origin.Y + shift.Y, Origin.Z + shift.Z);
	//FMath::Clamp(updatedLocation.Y, CameraFollowLocation.Y + CameraFollowExtents.Y, CameraFollowLocation.Y - CameraFollowExtents.Y);
	BoundingBox->SetWorldLocation(updatedLocation);
}
bool UCameraBoundingBoxComponent::CheckLevelBounds() {
	if (CameraFollowLocation != FVector::ZeroVector) {
		FMath::Clamp(Origin.Y + shift.Y, CameraFollowLocation.Y + CameraFollowExtents.Y, CameraFollowLocation.Y - CameraFollowExtents.Y);
		if (Origin.Y + shift.Y < (CameraFollowLocation.Y + CameraFollowExtents.Y)) {
			return false;
		}
		else if (Origin.Y - shift.Y > (CameraFollowLocation.Y - CameraFollowExtents.Y)) {
			return false;
		}

	}
	return true;
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
	FString resetString = "I Should probably add someinformation here: " +FString::SanitizeFloat(halfHeight) + " " + FString::FromInt(BoxExtent.Z) + " " + OffsetGround.ToCompactString();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, resetString);
	UE_LOG(LogClass, Log, TEXT("%s"), *resetString);
	BoundingBox->SetWorldLocation(OffsetGround);	
}