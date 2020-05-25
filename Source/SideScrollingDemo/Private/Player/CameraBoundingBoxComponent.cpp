// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraBoundingBoxComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "SideScrollingDemoTypes.h"
#include "LevelCameraFollowBounds.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"

// Sets default values for this component's properties
UCameraBoundingBoxComponent::UCameraBoundingBoxComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	

	
	BoundingBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Bounds"));

	BoundingBox->SetRelativeLocation(FVector(0.f, 0.f,0.f));
	BoundingBox->InitBoxExtent(MainBoxSize);
	BoundingBox->bHiddenInGame = false;
	BoundingBox->bVisible = true;
	BoundingBox->bAbsoluteRotation = true;
	BoundingBox->bAbsoluteLocation = true;
	//BoundingBox->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0.f, 0.f, 180.f)));
	//BoundingBox->bAbsoluteRotation = true;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(BoundingBox);
	CameraComponent->PostProcessSettings.AutoExposureMinBrightness = 1.f;
	CameraComponent->PostProcessSettings.AutoExposureMaxBrightness = 1.f;


	BlendFunc = (uint8)EViewTargetBlendFunction::VTBlend_Linear;
	
	// Have a Camera Transform in the blueprints its not necessary
	//CameraComponent->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
	BoundingBox->OnComponentBeginOverlap.AddDynamic(this, &UCameraBoundingBoxComponent::OnBoundingBoxOverlapBegin);
	BoundingBox->OnComponentEndOverlap.AddDynamic(this, &UCameraBoundingBoxComponent::OnBoundingBoxOverlapEnd);
	BoundingBox->OnComponentHit.AddDynamic(this, &UCameraBoundingBoxComponent::OnHit);
	

	BoxExtent = BoundingBox->GetScaledBoxExtent();
	
	MainCameraTransform.SetLocation(FVector(1000.f, -250.f, 0.f));
	MainCameraTransform.SetRotation(FQuat::MakeFromEuler(FVector(0.f, 0.f, -180.f)));
	MainCameraTransform.SetScale3D(FVector(1.f));

	CaveCameraTransform.SetLocation(FVector(750.f, -250.f, 0.f));
	CaveCameraTransform.SetRotation(FQuat::MakeFromEuler(FVector(0.f, 0.f, -180.f)));
	CaveCameraTransform.SetScale3D(FVector(1.f));


}
void UCameraBoundingBoxComponent::OnConstructionComponent() {
	//SetCameraMode(ECameraMode::MAIN);
	//BoundingBox->SetBoxExtent(MainBoxSize);
	CameraTransform = MainCameraTransform;
	CameraComponent->SetRelativeTransform(MainCameraTransform);
	CameraComponent->SetFieldOfView(FoV);

}

void UCameraBoundingBoxComponent::InitializeComponent() {
	Super::InitializeComponent();
	UE_LOG(LogClass, Log, TEXT("Initialized Component"));
}
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
	BoundingBox->SetBoxExtent(BoxSize);
	CameraComponent->SetRelativeTransform(CameraTransform);
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

void UCameraBoundingBoxComponent::OnSSDCharacterBeginPlay(UCapsuleComponent* targetCapsule){
	if(targetCapsule == NULL){
		return;
	}
	CameraTransform = MainCameraTransform;
	CameraComponent->SetRelativeTransform(MainCameraTransform);
	CameraComponent->SetFieldOfView(FoV);
	Origin = BoundingBox->GetComponentLocation();
	halfHeight = targetCapsule->GetScaledCapsuleHalfHeight();
	radius = targetCapsule->GetScaledCapsuleRadius();
	
	FString tmp = "Camera" + CameraComponent->GetRelativeLocation().ToCompactString() + "\nOrigin of the bounding box " + Origin.ToCompactString();
	UE_LOG(LogClass, Log, TEXT("Camera Bounding Box Information %s"), *tmp);

}
void UCameraBoundingBoxComponent::OnBoundingBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	// Maybe using tags for everything isn't idea but before I start effecting performance you need to have something made
	if (OtherComp && OtherComp->ComponentHasTag(ECustomTags::LevelBoundsTag)) {
		SetLevelBounds(OtherComp);
		/*UBoxComponent* NewBounds = Cast<UBoxComponent>(OtherComp);
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
		}*/
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
	if (OtherComp && OtherComp->ComponentHasTag(ECustomTags::LevelBoundsTag)) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Hit the level bounds");
	}
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

void UCameraBoundingBoxComponent::UpdatePosition(UCapsuleComponent* targetCapsule){
	if (targetCapsule == NULL) {
		return;
	}
	TargetLocation = targetCapsule->GetComponentLocation();


	targetLeft = TargetLocation.Y + radius;
	targetRight = TargetLocation.Y - radius;
	targetTop = TargetLocation.Z + halfHeight;
	targetBottom = TargetLocation.Z - halfHeight;

	//Origin = this->GetActorLocation();
	Origin  = BoundingBox->GetComponentLocation();
	BoxExtent = BoundingBox->GetScaledBoxExtent();
	
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



	//float shiftY = 0;
	//float shiftZ = 0;
	shift.Y = 0;
	shift.Z = 0;

	// Has to do with dumb orientation I need to fix that
	/*if (targetRight > levelBoundsRight && targetRight < right) {
		shift.Y = targetRight - right;
	}
	else if (targetLeft < levelBoundsLeft && targetLeft > left) {
		shift.Y = targetLeft - left;
	}
	left += shift.Y;
	right += shift.Y;

	if (targetBottom > levelBoundsBottom && targetBottom < bottom) {
		shift.Z = targetBottom - bottom;
	}
	else if (targetTop < levelBoundsTop && targetTop > top) {
		shift.Z = targetTop - top;

	}*/
	// Theres a certain point where the sign changes, 1 -> 0 -> -1, thats a headache but whatever
	if(targetRight < right) shift.Y = targetRight - right;
	else if(targetLeft > left) shift.Y = targetLeft - left;

	if(targetBottom < bottom) shift.Z = targetBottom - bottom;
	else if(targetTop > top) shift.Z = targetTop - top;

	//UE_LOG(LogClass, Log, TEXT("Camera Component Update Location %s"), *shift.ToCompactString());

	//this->SetActorLocation(FVector(Origin.X, Origin.Y + shiftY, Origin.Z + shiftZ));
	FVector updatedLocation = FVector(0.f, Origin.Y + shift.Y, Origin.Z + shift.Z);
	//FMath::Clamp(updatedLocation.Y, CameraFollowLocation.Y + CameraFollowExtents.Y, CameraFollowLocation.Y - CameraFollowExtents.Y);
	//UE_LOG(LogClass, Log, TEXT("Camera Component Update Location %s"), *updatedLocation.ToCompactString());
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
void UCameraBoundingBoxComponent::InitializePosition(APlayerController*  PlayerController, AActor* ActorInFocus, UCapsuleComponent* targetCapsule) {
	BoundingBox->SetWorldLocation(targetCapsule->GetComponentLocation());
	PlayerController->SetViewTargetWithBlend(ActorInFocus, BlendTime, (EViewTargetBlendFunction)BlendFunc, BlendExp);

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
	BoundingBox->SetWorldLocation(OffsetGround);	
}