// Fill out your copyright notice in the Description page of Project Settings.


#include "World_Actors/Wall_Pipe_Actor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Character/Technical_Animator_Character.h"
#include "Debug/DebugHelper.h"



// Sets default values
AWall_Pipe_Actor::AWall_Pipe_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//Set this actor to replicate. This will give the server authority.
	bReplicates = true;

	//Create the scene component which will be the root component.
	Default_Scene_Root = CreateDefaultSubobject<USceneComponent>(FName(TEXT("Default_Scene_Root")));
	SetRootComponent(Default_Scene_Root);

	Area_Capsule = CreateDefaultSubobject<UCapsuleComponent>(FName(TEXT("Area_Capsule")));
	Area_Capsule->SetupAttachment(RootComponent);
	Area_Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Area_Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	Display_Widget = CreateDefaultSubobject<UWidgetComponent>(FName(TEXT("Display_Widget")));
	Display_Widget->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AWall_Pipe_Actor::BeginPlay()
{
	Super::BeginPlay();

	//To have the server maintain as much authority over the game all overlap events are bound only on the server. All overlap events variables will replicate from server to clinets when nessesary.
	//Hence &AWall_Pipe_Actor::On_Capsule_Overlap and &AWall_Pipe_Actor::On_Capsule_End_Overlap will only be called on the server when clients begin overlapping with the "Area_Capsule".
	if(HasAuthority() && Area_Capsule)
	{
		Area_Capsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Area_Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		Area_Capsule->OnComponentBeginOverlap.AddDynamic(this, &AWall_Pipe_Actor::On_Capsule_Begin_Overlap);
		Area_Capsule->OnComponentEndOverlap.AddDynamic(this, &AWall_Pipe_Actor::On_Capsule_End_Overlap);
	}

	//Sets the visibility on the "Display_Widget" for the server and the clients to false.
	if(Display_Widget)
	{
		Display_Widget->SetVisibility(false);
	}

	Generate_Custom_Wall_Pipe_Actor_Forward_Vector();

}

// Called every frame
void AWall_Pipe_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


#pragma region Helper_Functions

FVector AWall_Pipe_Actor::Move_Vector_Down(const FVector& Initial_Location, const float& Move_Value) const
{
	const FVector Move_Direction{-UKismetMathLibrary::GetUpVector(GetActorRotation())};
	const FVector Destination{Initial_Location + (Move_Direction * Move_Value)};
	
	return Destination;
}

FVector AWall_Pipe_Actor::Move_Vector_Forward(const FVector& Initial_Location, const FRotator& Rotation, const float& Move_Value)
{
	const FVector Move_Direction{UKismetMathLibrary::GetForwardVector(Rotation)};
	const FVector Destination{Initial_Location + (Move_Direction * Move_Value)};

	return Destination;
}

#pragma endregion


#pragma region Ray_Casts

void AWall_Pipe_Actor::Generate_Custom_Wall_Pipe_Actor_Forward_Vector()
{
	const FVector Actor_Location{GetActorLocation()};

	const FRotator Actor_Rotation{GetActorRotation()};


	const FVector Offset_Vector_Down{Move_Vector_Down(Actor_Location, 7.f)};

	const FVector Start{Offset_Vector_Down};
	
	const FVector End{Move_Vector_Forward(Start, Actor_Rotation, 75.f)};
	
	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		Start,
		End,
		Wall_Pipe_Forward_Vector_Trace_Types,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::ForOneFrame,
		Custom_Wall_Pipe_Actor_Forward_Vector_Hit_Result,
		false
	);
	
}

#pragma endregion


#pragma region Delegate_Callbacks

void AWall_Pipe_Actor::On_Capsule_Begin_Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ATechnical_Animator_Character* Overlapping_Character{Cast<ATechnical_Animator_Character>(OtherActor)};

	if(Overlapping_Character)
	Overlapping_Character->Set_Overlapping_Wall_Pipe_Actor(this);
}

void AWall_Pipe_Actor::On_Capsule_End_Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ATechnical_Animator_Character* Overlapping_Character{Cast<ATechnical_Animator_Character>(OtherActor)};

	if(Overlapping_Character)
	Overlapping_Character->Set_Overlapping_Wall_Pipe_Actor(nullptr);
}

void AWall_Pipe_Actor::Set_Wall_Pipe_Actor_Widget_Visibility(const bool& bDisplay_Widget)
{
	if(Display_Widget)
	Display_Widget->SetVisibility(bDisplay_Widget);
}

#pragma endregion
