// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraBoundingBoxComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "SideScrollingDemoTypes.h"
#include "LevelCameraFollowBounds.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"
#include "Components/StaticMeshComponent.h"
#include "Engine.h"

// Sets default values for this component's properties
UCameraBoundingBoxComponent::UCameraBoundingBoxComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	

	
	BoundingBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Bounds"));

	verify(BoundingBox->IsValidLowLevel());

	GetBoundingBox()->SetRelativeLocation(FVector(0.f, 0.f,0.f));
	GetBoundingBox()->InitBoxExtent(MainBoxSize);

	GetBoundingBox()->SetUsingAbsoluteLocation(true);
	GetBoundingBox()->SetUsingAbsoluteRotation(true);
	GetBoundingBox()->bHiddenInGame = true;
	//BoundingBox->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0.f, 0.f, 180.f)));
	//BoundingBox->bAbsoluteRotation = true;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	verify(CameraComponent->IsValidLowLevel());

	GetCameraComponent()->SetupAttachment(BoundingBox);
	GetCameraComponent()->PostProcessSettings.AutoExposureMinBrightness = 1.f;
	GetCameraComponent()->PostProcessSettings.AutoExposureMaxBrightness = 1.f;


	BlendFunc = (uint8)EViewTargetBlendFunction::VTBlend_Linear;
	
	// Have a Camera Transform in the blueprints its not necessary
	//CameraComponent->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
	GetBoundingBox()->OnComponentBeginOverlap.AddDynamic(this, &UCameraBoundingBoxComponent::OnBoundingBoxOverlapBegin);
	GetBoundingBox()->OnComponentEndOverlap.AddDynamic(this, &UCameraBoundingBoxComponent::OnBoundingBoxOverlapEnd);
	GetBoundingBox()->OnComponentHit.AddDynamic(this, &UCameraBoundingBoxComponent::OnHit);
	

	BoxExtent = GetBoundingBox()->GetScaledBoxExtent();
	
	MainCameraTransform.SetLocation(FVector(1000.f, -250.f, -50.f));
	MainCameraTransform.SetRotation(FQuat::MakeFromEuler(FVector(0.f, 0.f, -180.f)));
	MainCameraTransform.SetScale3D(FVector(1.f));

	CaveCameraTransform.SetLocation(FVector(750.f, -250.f, -25.f));
	CaveCameraTransform.SetRotation(FQuat::MakeFromEuler(FVector(0.f, 0.f, -180.f)));
	CaveCameraTransform.SetScale3D(FVector(1.f));

	ViewPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ViewPlane"));
	
	verify(ViewPlane->IsValidLowLevel());

	GetViewPlane()->SetupAttachment(BoundingBox);
	GetViewPlane()->SetRelativeLocation(FVector(15.f, 0.f, 0.f));
	GetViewPlane()->SetRelativeRotation(FRotator(0.f, 90.f, 180.f));
	//GetViewPlane()->SetStaticMesh()

}
/*void UCameraBoundingBoxComponent::OnConstructionComponent() {
	//SetCameraMode(ECameraMode::MAIN);
	//BoundingBox->SetBoxExtent(MainBoxSize);


}

void UCameraBoundingBoxComponent::InitializeComponent() {
	Super::InitializeComponent();
	CameraTransform = MainCameraTransform;
	CameraComponent->SetRelativeTransform(MainCameraTransform);
	CameraComponent->SetFieldOfView(FoV);
	UE_LOG(LogClass, Log, TEXT("Initialized Component"));
}*/
// This works but requires a smoother transition perhaps a timeline
void UCameraBoundingBoxComponent::SetCameraMode(ECameraMode mode) {
	// There should be a timeline for transitions here
	switch (mode) {
		case MAIN:
			BoxSize = MainBoxSize;
			CameraTransform = MainCameraTransform;
			break;
		case CAVE:
			BoxSize = CaveBoxSize;
			CameraTransform = CaveCameraTransform;
			break;
	}
	GetBoundingBox()->SetBoxExtent(BoxSize);
	GetCameraComponent()->SetRelativeTransform(CameraTransform);
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
void UCameraBoundingBoxComponent::InitializeCameraOverlapBounds(UPrimitiveComponent* OtherComp) {

}
void UCameraBoundingBoxComponent::InitializeViewPlane() {
	FVector viewPlaneLocation = MainCameraTransform.GetLocation();
	viewPlaneLocation.X = 0.f;
	GetViewPlane()->SetWorldLocation(viewPlaneLocation);
}
void::UCameraBoundingBoxComponent::MoveViewPlane() {
	FVector viewPlaneLocation = ViewPlane->GetRelativeLocation();
	viewPlaneLocation.X += 10.f;
	GetViewPlane()->SetRelativeLocation(viewPlaneLocation);
}
void UCameraBoundingBoxComponent::OnSSDCharacterBeginPlay(UCapsuleComponent* targetCapsule){
	if(targetCapsule == NULL || !ConfirmComponentValidLowLevel()){
		return;
	}
	CameraTransform = MainCameraTransform;
	GetCameraComponent()->SetRelativeTransform(MainCameraTransform);
	GetCameraComponent()->SetFieldOfView(FoV);
	Origin = GetBoundingBox()->GetComponentLocation();
	halfHeight = targetCapsule->GetScaledCapsuleHalfHeight();
	radius = targetCapsule->GetScaledCapsuleRadius();

	FVector viewPlaneVector = MainCameraTransform.GetLocation();
	viewPlaneVector.X -= 35.f;
	GetViewPlane()->SetRelativeLocation(viewPlaneVector);
	
	FString tmp = "Camera" + GetCameraComponent()->GetRelativeLocation().ToCompactString() + " Origin of the bounding box " + Origin.ToCompactString() +" ViewPlane: " + GetViewPlane()->GetComponentLocation().ToCompactString();
	UE_LOG(LogClass, Log, TEXT("Camera Bounding Box Information %s"), *tmp);

}
bool UCameraBoundingBoxComponent::ConfirmComponentValidLowLevel() {
	if (GetBoundingBox()->IsValidLowLevel() && GetCameraComponent()->IsValidLowLevel() && GetViewPlane()->IsValidLowLevel()) return true;
	return false;
}
void UCameraBoundingBoxComponent::OnBoundingBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	// Maybe using tags for everything isn't idea but before I start effecting performance you need to have something made
	if (OtherComp && OtherComp->ComponentHasTag(ECustomTags::LevelBoundsTag)) {
		SetLevelBounds(OtherComp);
	}
}
void UCameraBoundingBoxComponent::OnBoundingBoxOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

	if (OtherComp && OtherComp->ComponentHasTag(ECustomTags::LevelBoundsTag)) {
		//SetLevelBounds(OtherComp);
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
	}
}
void UCameraBoundingBoxComponent::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	/*if (OtherComp && OtherComp->ComponentHasTag(ECustomTags::LevelBoundsTag)) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Hit the level bounds");
	}*/
}
void UCameraBoundingBoxComponent::SetLevelBounds(UPrimitiveComponent* Bounds) {
	UBoxComponent* NewBounds = Cast<UBoxComponent>(Bounds);
	ALevelCameraFollowBounds* levelBounds = Cast<ALevelCameraFollowBounds>(Bounds->GetAttachmentRootActor());
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

void UCameraBoundingBoxComponent::UpdatePosition(FVector TargetLocation, float DeltaTime){
	targetLeft = TargetLocation.Y + radius;
	targetRight = TargetLocation.Y - radius;
	targetTop = TargetLocation.Z + halfHeight;
	targetBottom = TargetLocation.Z - halfHeight;

	//Origin = this->GetActorLocation();
	Origin  = GetBoundingBox()->GetComponentLocation();
	BoxExtent = GetBoundingBox()->GetScaledBoxExtent();
	
	cameraBoxRight = Origin.Y - BoxExtent.Y; // The sign has to do with the way the screen is oriented
	cameraBoxLeft = Origin.Y + BoxExtent.Y;
	cameraBoxTop = Origin.Z + BoxExtent.Z;
	cameraBoxBottom = Origin.Z - BoxExtent.Z;

	//FString tmpBounds = CameraFollowLocation.ToCompactString() + " " + CameraFollowExtents.ToCompactString();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, tmpBounds);
	
	float levelBoundsRight = CameraFollowLocation.Y - CameraFollowExtents.Y;
	float levelBoundsLeft = CameraFollowLocation.Y + CameraFollowExtents.Y;
	float levelBoundsTop = CameraFollowLocation.Z + CameraFollowExtents.Z;
	float levelBoundsBottom = CameraFollowLocation.Z - CameraFollowExtents.Z;



	//float shiftY = 0;
	//float shiftZ = 0;
	shift.Y = 0;
	shift.Z = 0;

	// Has to do with dumb orientation I need to fix that
	if (targetRight > levelBoundsRight && targetRight < cameraBoxRight) {
		shift.Y = targetRight - cameraBoxRight;
	}
	else if (targetLeft < levelBoundsLeft && targetLeft > cameraBoxLeft) {
		shift.Y = targetLeft - cameraBoxLeft;
	}
	cameraBoxLeft += shift.Y;
	cameraBoxRight += shift.Y;

	if (targetBottom > levelBoundsBottom && targetBottom < cameraBoxBottom) {
		shift.Z = targetBottom - cameraBoxBottom;
	}
	else if (targetTop < levelBoundsTop && targetTop > cameraBoxTop) {
		shift.Z = targetTop - cameraBoxTop;

	}
	if (lockCameraToBottom && targetBottom != cameraBoxBottom && CharacterWithinLevelBounds()) {
		shift.Z = (targetBottom - cameraBoxBottom);
		if (FMath::Abs(shift.Z) >= maxDistanceLimiter) {
			shift.Z *= 3*DeltaTime;
		}
		else {
			shift.Z = (targetBottom - cameraBoxBottom);
		}
	}

	// Theres a certain point where the sign changes, 1 -> 0 -> -1, thats a headache but whatever
	/*if(targetRight < right) shift.Y = targetRight - right;
	else if(targetLeft > left) shift.Y = targetLeft - left;

	if(targetBottom < bottom) shift.Z = targetBottom - bottom;
	else if(targetTop > top) shift.Z = targetTop - top;
	*/
	//UE_LOG(LogClass, Log, TEXT("Camera Component Update Location %s"), *shift.ToCompactString());

	//this->SetActorLocation(FVector(Origin.X, Origin.Y + shiftY, Origin.Z + shiftZ));
	FVector updatedLocation = FVector(0.f, Origin.Y + shift.Y, Origin.Z + shift.Z);
	//FMath::Clamp(updatedLocation.Y, CameraFollowLocation.Y + CameraFollowExtents.Y, CameraFollowLocation.Y - CameraFollowExtents.Y);
	//UE_LOG(LogClass, Log, TEXT("Camera Component Update Location %s"), *updatedLocation.ToCompactString());
	GetBoundingBox()->SetWorldLocation(updatedLocation);
}
bool UCameraBoundingBoxComponent::CharacterWithinLevelBounds() {
	// Calculating the same values twice, poor form, pass them in as function signature?		
	float levelBoundsRight = CameraFollowLocation.Y - CameraFollowExtents.Y;
	float levelBoundsLeft = CameraFollowLocation.Y + CameraFollowExtents.Y;
	float levelBoundsTop = CameraFollowLocation.Z + CameraFollowExtents.Z;
	float levelBoundsBottom = CameraFollowLocation.Z - CameraFollowExtents.Z;


	// -Y direction really being annoying
	if (levelBoundsLeft > cameraBoxLeft && cameraBoxRight > levelBoundsRight && cameraBoxBottom > levelBoundsBottom && cameraBoxTop < levelBoundsTop) {
		return true;
	}
	return false;
}
void UCameraBoundingBoxComponent::InitializePosition(APlayerController*  PlayerController, AActor* ActorInFocus, UCapsuleComponent* targetCapsule) {
	GetBoundingBox()->SetWorldLocation(targetCapsule->GetComponentLocation());
	if (PlayerController->IsValidLowLevel()) {
		PlayerController->SetViewTargetWithBlend(ActorInFocus, BlendTime, (EViewTargetBlendFunction)BlendFunc, BlendExp);
	}
}
void UCameraBoundingBoxComponent::ResetCamera(AActor* targetActor) {

	TargetLocation = targetActor->GetActorLocation();
	//halfHeight = targetActor->GetSimpleCollisionHalfHeight();
	//radius = targetActor->GetSimpleCollisionRadius();
	
	FVector OffsetGround = TargetLocation;


	OffsetGround.Z += (BoxExtent.Z - halfHeight);
	FString resetString = "Reset Camera has been called: " +FString::SanitizeFloat(halfHeight) + " " + FString::FromInt(BoxExtent.Z) + " " + OffsetGround.ToCompactString();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, resetString);
	UE_LOG(LogClass, Log, TEXT("%s"), *resetString);
	GetBoundingBox()->SetWorldLocation(OffsetGround);	
}
void UCameraBoundingBoxComponent::LogAtReducedRate(int counter, FString Log) {
	reducedRateLoggingCounter++;
	/*if (targetBottom < bottom) {
		FString tmp = "Bottom: " + FString::SanitizeFloat(bottom) + " TargetBottom: " + FString::SanitizeFloat(targetBottom) + " ShiftZ: " + FString::SanitizeFloat(shift.Z);
		if (reducedRateLoggingCounter % 10 == 0) {
			reducedRateLoggingCounter = 0;
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, tmp);
		}
	}*/
}

void UCameraBoundingBoxComponent::setLockCameraToBottom(bool setLock) { lockCameraToBottom = setLock; }
