// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoint.h"
#include "Components/BoxComponent.h"
#include "SideScrollingDemoTypes.h"
#include "SSDCharacter.h"

// Sets default values
ACheckpoint::ACheckpoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CheckTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("CheckpointTrigger"));
	CheckTrigger->InitBoxExtent(BoxExtent);

	CheckTrigger->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnCheckOverlapBegin);
	CheckTrigger->OnComponentEndOverlap.AddDynamic(this, &ACheckpoint::OnCheckOverlapEnd);

}

// Called when the game starts or when spawned
void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACheckpoint::OnCheckOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor && OtherActor->ActorHasTag(ECustomTags::PlayerTag)) {
		ASSDCharacter* player = Cast<ASSDCharacter>(OtherActor);
		if (player) {
			UE_LOG(LogClass, Log, TEXT("Hellloooooo Nurse"));
			PlayerLocation = player->GetActorLocation();
		}
	}

}

void ACheckpoint::OnCheckOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

}
