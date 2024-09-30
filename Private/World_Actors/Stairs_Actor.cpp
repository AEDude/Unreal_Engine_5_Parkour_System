// Fill out your copyright notice in the Description page of Project Settings.


#include "World_Actors/Stairs_Actor.h"
#include "Components/BoxComponent.h"
#include "Character/Technical_Animator_Character.h"

// Sets default values
AStairs_Actor::AStairs_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Set this actor to replicate. This will give the server authority.
	bReplicates = true;

	Area_Box = CreateDefaultSubobject<UBoxComponent>(FName(TEXT("Area Box")));
	Area_Box->SetupAttachment(RootComponent);
	Area_Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Area_Box->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

}

// Called when the game starts or when spawned
void AStairs_Actor::BeginPlay()
{
	Super::BeginPlay();

	if(Area_Box && HasAuthority())
	{
		Area_Box->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		Area_Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		Area_Box->OnComponentBeginOverlap.AddDynamic(this, &AStairs_Actor::On_Box_Begin_Overlap);
		Area_Box->OnComponentEndOverlap.AddDynamic(this, &AStairs_Actor::On_Box_End_Overlap);

	}
	
}

// Called every frame
void AStairs_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


#pragma region delegate callbacks

void AStairs_Actor::On_Box_Begin_Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ATechnical_Animator_Character* Overlapping_Actor{Cast<ATechnical_Animator_Character>(OtherActor)};

	if(Overlapping_Actor)
	{
		Overlapping_Actor->Set_Overlapping_Stairs_Actor(this);

	}

}

void AStairs_Actor::On_Box_End_Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ATechnical_Animator_Character* Overlapping_Actor{Cast<ATechnical_Animator_Character>(OtherActor)};

	if(Overlapping_Actor)
	{
		Overlapping_Actor->Set_Overlapping_Stairs_Actor(nullptr);

	}

}

#pragma region endregion