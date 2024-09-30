// Fill out your copyright notice in the Description page of Project Settings.


#include "World_Actors/Balance_Traversal_Actor.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Character/Technical_Animator_Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ABalance_Traversal_Actor::ABalance_Traversal_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	//Set this actor to replicate. This will give the server authority.
	bReplicates = true;

	Default_Scene_Root = CreateDefaultSubobject<USceneComponent>(FName(TEXT("Default_Scene_Root")));
	SetRootComponent(Default_Scene_Root);

	Area_Box = CreateDefaultSubobject<UBoxComponent>(FName(TEXT("Area_Box")));
	Area_Box->SetupAttachment(RootComponent);
	Area_Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Area_Box->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	Display_Widget = CreateDefaultSubobject<UWidgetComponent>(FName(TEXT("Display_Widget")));
	Display_Widget->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABalance_Traversal_Actor::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority() && Area_Box)
	{
		Area_Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Area_Box->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		Area_Box->OnComponentBeginOverlap.AddDynamic(this, &ABalance_Traversal_Actor::On_Box_Begin_Overlap);
		Area_Box->OnComponentEndOverlap.AddDynamic(this, &ABalance_Traversal_Actor::On_Box_End_Overlap);
	}

	if(Display_Widget)
	{
		Display_Widget->SetVisibility(false);
	}

	Balance_Traversal_Actor_Forward_Vector = GetActorLocation() + GetActorForwardVector() * 1;
	
}

// Called every frame
void ABalance_Traversal_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

#pragma region Helper_Functions

FVector ABalance_Traversal_Actor::Move_Vector_Forward(const FVector& Current_Location, const FRotator& Current_Rotation, const float& Move_Value)
{
	const FVector Forward_Vector_Of_Current_Rotation{UKismetMathLibrary::GetForwardVector(Current_Rotation)};

	const FVector Location_To_Move_Vector{Current_Location + (Forward_Vector_Of_Current_Rotation * Move_Value)};

	return Location_To_Move_Vector;
}

#pragma endregion

#pragma region Traces

void ABalance_Traversal_Actor::Generate_Custom_Wall_Pipe_Actor_Forward_Vector()
{
	const FVector Actor_Location{GetActorLocation()};
	const FRotator Actor_Rotation{GetActorRotation()};

	const FVector Start{Actor_Location};
	const FVector End{Move_Vector_Forward(Start, Actor_Rotation, 700000.f)};
	
	FHitResult Custom_Balance_Traversal_Actor_Forward_Vector_Hit_Result{};

	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		Start,
		End,
		Balance_Traversal_Forward_Vector_Trace_Types,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::ForOneFrame,
		Custom_Balance_Traversal_Actor_Forward_Vector_Hit_Result,
		false
	);
}

#pragma endregion

#pragma region Delegate_Callbacks

void ABalance_Traversal_Actor::On_Box_Begin_Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ATechnical_Animator_Character* Overlapping_Character{Cast<ATechnical_Animator_Character>(OtherActor)};

	//Show the "Display Widget" on the owning character that is overlapping with the Box Component.
	if(Overlapping_Character)
	{
		Overlapping_Character->Set_Overlapping_Balance_Traversal_Actor(this);
	}

}

void ABalance_Traversal_Actor::On_Box_End_Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ATechnical_Animator_Character* Overlapping_Character{Cast<ATechnical_Animator_Character>(OtherActor)};
	
	//Hide the "Display Widget" on the owning character that is overlapping with the Box Component.
	if(Overlapping_Character)
	{
		Overlapping_Character->Set_Overlapping_Balance_Traversal_Actor(nullptr);
	}
}

void ABalance_Traversal_Actor::Show_Display_Widget(const bool& bShow_Display_Widget)
{
	if(Display_Widget)
	{
		Display_Widget->SetVisibility(bShow_Display_Widget);
	}
}

#pragma endregion