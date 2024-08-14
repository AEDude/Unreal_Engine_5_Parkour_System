// Fill out your copyright notice in the Description page of Project Settings.


#include "World_Actors/Tic_Tac_Actor.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Character/Technical_Animator_Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ATic_Tac_Actor::ATic_Tac_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root_Component = CreateDefaultSubobject<USceneComponent>(FName(TEXT("Scene Root")));
	SetRootComponent(Root_Component);

	Area_Box_1 = CreateDefaultSubobject<UBoxComponent>(FName(TEXT("Area Box 1")));
	Area_Box_1->SetupAttachment(RootComponent);
	Area_Box_1->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Area_Box_1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	
	Area_Box_2 = CreateDefaultSubobject<UBoxComponent>(FName(TEXT("Area Box 2")));
	Area_Box_2->SetupAttachment(RootComponent);
	Area_Box_2->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Area_Box_2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	

	Area_Box_3 = CreateDefaultSubobject<UBoxComponent>(FName(TEXT("Area Box 3")));
	Area_Box_3->SetupAttachment(RootComponent);
	Area_Box_3->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Area_Box_3->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	


	Display_Widget = CreateDefaultSubobject<UWidgetComponent>(FName(TEXT("Display Widget")));
	Display_Widget->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ATic_Tac_Actor::BeginPlay()
{
	Super::BeginPlay();

	if(Area_Box_1 && Area_Box_2 && Area_Box_3 && HasAuthority())
	{
		Area_Box_1->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Area_Box_1->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		Area_Box_1->OnComponentBeginOverlap.AddDynamic(this, &ATic_Tac_Actor::On_Box_1_Begin_Overlap);
		Area_Box_1->OnComponentEndOverlap.AddDynamic(this, &ATic_Tac_Actor::On_Box_1_End_Overlap);

		Area_Box_2->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Area_Box_2->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		Area_Box_2->OnComponentBeginOverlap.AddDynamic(this, &ATic_Tac_Actor::On_Box_2_Begin_Overlap);
		Area_Box_2->OnComponentEndOverlap.AddDynamic(this, &ATic_Tac_Actor::On_Box_2_End_Overlap);

		Area_Box_3->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Area_Box_3->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		Area_Box_3->OnComponentBeginOverlap.AddDynamic(this, &ATic_Tac_Actor::On_Box_3_Begin_Overlap);
		Area_Box_3->OnComponentEndOverlap.AddDynamic(this, &ATic_Tac_Actor::On_Box_3_End_Overlap);
	}

	if(Display_Widget)
	{
		Display_Widget->SetVisibility(false);
	}

	Generate_Tic_Tac_Front_Wall_Hit_Result();
	
}

// Called every frame
void ATic_Tac_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


#pragma region Delegate_Callbacks

void ATic_Tac_Actor::On_Box_1_Begin_Overlap(UPrimitiveComponent* OverlappedComponent, 
										  AActor* OtherActor, 
										  UPrimitiveComponent* OtherComp, 
										  int32 OtherBodyIndex, 
										  bool bFromSweep, 
										  const FHitResult& SweepResult)
{
	ATechnical_Animator_Character* Overlapping_Character{Cast<ATechnical_Animator_Character>(OtherActor)};

	//Show the "Display Widget" on the owning character that is overlapping with the Box Component.
	if(Overlapping_Character)
	{
		Overlapping_Character->Set_Overlapping_Tic_Tac_Actor(this, 1);
	}

}

void ATic_Tac_Actor::On_Box_1_End_Overlap(UPrimitiveComponent* OverlappedComponent, 
										AActor* OtherActor, 
										UPrimitiveComponent* OtherComp, 
										int32 OtherBodyIndex)
{
	ATechnical_Animator_Character* Overlapping_Character{Cast<ATechnical_Animator_Character>(OtherActor)};

	//Show the "Display Widget" on the owning character that is overlapping with the Box Component.
	if(Overlapping_Character)
	{
		Overlapping_Character->Set_Overlapping_Tic_Tac_Actor(nullptr, 0);
	}

}

void ATic_Tac_Actor::On_Box_2_Begin_Overlap(UPrimitiveComponent* OverlappedComponent, 
										  AActor* OtherActor, 
										  UPrimitiveComponent* OtherComp, 
										  int32 OtherBodyIndex, 
										  bool bFromSweep, 
										  const FHitResult& SweepResult)
{
	ATechnical_Animator_Character* Overlapping_Character{Cast<ATechnical_Animator_Character>(OtherActor)};

	//Show the "Display Widget" on the owning character that is overlapping with the Box Component.
	if(Overlapping_Character)
	{
		Overlapping_Character->Set_Overlapping_Tic_Tac_Actor(this, 2);
	}

}

void ATic_Tac_Actor::On_Box_2_End_Overlap(UPrimitiveComponent* OverlappedComponent, 
										AActor* OtherActor, 
										UPrimitiveComponent* OtherComp, 
										int32 OtherBodyIndex)
{
	ATechnical_Animator_Character* Overlapping_Character{Cast<ATechnical_Animator_Character>(OtherActor)};

	//Show the "Display Widget" on the owning character that is overlapping with the Box Component.
	if(Overlapping_Character)
	{
		Overlapping_Character->Set_Overlapping_Tic_Tac_Actor(nullptr, 0);
	}

}

void ATic_Tac_Actor::On_Box_3_Begin_Overlap(UPrimitiveComponent* OverlappedComponent, 
										  AActor* OtherActor, 
										  UPrimitiveComponent* OtherComp, 
										  int32 OtherBodyIndex, 
										  bool bFromSweep, 
										  const FHitResult& SweepResult)
{
	ATechnical_Animator_Character* Overlapping_Character{Cast<ATechnical_Animator_Character>(OtherActor)};

	//Show the "Display Widget" on the owning character that is overlapping with the Box Component.
	if(Overlapping_Character)
	{
		Overlapping_Character->Set_Overlapping_Tic_Tac_Actor(this, 3);
	}

}

void ATic_Tac_Actor::On_Box_3_End_Overlap(UPrimitiveComponent* OverlappedComponent, 
										AActor* OtherActor, 
										UPrimitiveComponent* OtherComp, 
										int32 OtherBodyIndex)
{
	ATechnical_Animator_Character* Overlapping_Character{Cast<ATechnical_Animator_Character>(OtherActor)};

	//Show the "Display Widget" on the owning character that is overlapping with the Box Component.
	if(Overlapping_Character)
	{
		Overlapping_Character->Set_Overlapping_Tic_Tac_Actor(nullptr, 0);
	}

}

#pragma endregion

#pragma region Helper_Functions

FVector ATic_Tac_Actor::Move_Vector_Up(const FVector& Initial_Location, const float& Move_Value) const
{
	const FVector Move_Direction{GetActorUpVector()};
	const FVector Destination{Initial_Location + (Move_Direction * Move_Value)};
	
	return Destination;
}

FVector ATic_Tac_Actor::Move_Vector_Down(const FVector& Initial_Location, const float& Move_Value) const
{
	const FVector Move_Direction{-GetActorUpVector()};
	const FVector Destination{Initial_Location + (Move_Direction * Move_Value)};
	
	return Destination;
}

FVector ATic_Tac_Actor::Move_Vector_Backward(const FVector& Initial_Location, const FRotator& Rotation, const float& Move_Value) const
{
	const FVector Forward_Vector{-UKismetMathLibrary::GetForwardVector(Rotation)};
	const FVector Destination{Initial_Location + (Forward_Vector * Move_Value)};

	return Destination;
}

#pragma endregion

void ATic_Tac_Actor::Show_Display_Widget(const bool& bShow_Display_Widget)
{
	if(Display_Widget)
	{
		Display_Widget->SetVisibility(bShow_Display_Widget);
	}
}

void ATic_Tac_Actor::Generate_Tic_Tac_Front_Wall_Hit_Result()
{
	const FVector Tic_Tac_Actor_Location{GetActorLocation()};
	const FRotator Tic_Tac_Actor_Rotation{GetActorRotation()};

	const FVector Offset_Vector_Up{Move_Vector_Up(Tic_Tac_Actor_Location, 155.f)};

	FVector Start{};

	if(bCan_Tic_Tac_Over_Front_Wall)
	{
		Start = Move_Vector_Backward(Offset_Vector_Up, Tic_Tac_Actor_Rotation, 200.f);
	}

	else
	{
		Start = Move_Vector_Backward(Offset_Vector_Up, Tic_Tac_Actor_Rotation, 230.f);
	}

	const FVector End{Move_Vector_Down(Start, 40.f)};

	UKismetSystemLibrary::SphereTraceSingleForObjects(
		this,
		Start,
		End,
		5.f,
		Generate_Tic_Tac_Front_Wall_Hit_Result_trace_Types,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		Tic_Tac_Front_Wall_Hit_Result,
		false
	);
}