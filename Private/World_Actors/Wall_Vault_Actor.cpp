// Fill out your copyright notice in the Description page of Project Settings.


#include "World_Actors/Wall_Vault_Actor.h"
#include "Components/BoxComponent.h"
#include "Character/Technical_Animator_Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
AWall_Vault_Actor::AWall_Vault_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Default_Scene_Root = CreateDefaultSubobject<USceneComponent>(FName(TEXT("Scene_Root")));
	SetRootComponent(Default_Scene_Root);

	Area_Box_1 = CreateDefaultSubobject<UBoxComponent>(FName(TEXT("Area_Box_1")));
	Area_Box_1->SetupAttachment(Default_Scene_Root);
	Area_Box_1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Area_Box_1->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	Area_Box_2 = CreateDefaultSubobject<UBoxComponent>(FName(TEXT("Area_Box_2")));
	Area_Box_2->SetupAttachment(Default_Scene_Root);
	Area_Box_2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Area_Box_2->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	Area_Box_3 = CreateDefaultSubobject<UBoxComponent>(FName(TEXT("Area_Box_3")));
	Area_Box_3->SetupAttachment(Default_Scene_Root);
	Area_Box_3->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Area_Box_3->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	Area_Box_4 = CreateDefaultSubobject<UBoxComponent>(FName(TEXT("Area_Box_4")));
	Area_Box_4->SetupAttachment(Default_Scene_Root);
	Area_Box_4->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Area_Box_4->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	

}

// Called when the game starts or when spawned
void AWall_Vault_Actor::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority() && Area_Box_1 && Area_Box_2 && Area_Box_3 && Area_Box_4)
	{
		Area_Box_1->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Area_Box_1->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		Area_Box_1->OnComponentBeginOverlap.AddDynamic(this, &AWall_Vault_Actor::On_Box_Begin_Overlap);
		Area_Box_1->OnComponentEndOverlap.AddDynamic(this, &AWall_Vault_Actor::On_Box_End_Overlap);


		Area_Box_2->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Area_Box_2->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		Area_Box_2->OnComponentBeginOverlap.AddDynamic(this, &AWall_Vault_Actor::On_Box_Begin_Overlap);
		Area_Box_2->OnComponentEndOverlap.AddDynamic(this, &AWall_Vault_Actor::On_Box_End_Overlap);


		Area_Box_3->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Area_Box_3->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		Area_Box_3->OnComponentBeginOverlap.AddDynamic(this, &AWall_Vault_Actor::On_Box_Begin_Overlap);
		Area_Box_3->OnComponentEndOverlap.AddDynamic(this, &AWall_Vault_Actor::On_Box_End_Overlap);


		Area_Box_4->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Area_Box_4->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		Area_Box_4->OnComponentBeginOverlap.AddDynamic(this, &AWall_Vault_Actor::On_Box_Begin_Overlap);
		Area_Box_4->OnComponentEndOverlap.AddDynamic(this, &AWall_Vault_Actor::On_Box_End_Overlap);
	}

	if(bEnable_Wall_Under_Bar)
	{
		Obtain_Wall_Top_Result();
	}
	
}

// Called every frame
void AWall_Vault_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


#pragma region Delegate_Callbacks

void AWall_Vault_Actor::On_Box_Begin_Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//Cast the input argument "OtherActor" to the class ATechnical_Animator_Character to retrieve a reference to the character class which is overlapping with the "Area_Box".

	ATechnical_Animator_Character* Overlapping_Actor{Cast<ATechnical_Animator_Character>(OtherActor)};

	if(Overlapping_Actor)
	Overlapping_Actor->Set_Overlapping_Wall_Vault_Actor(this);
}

void AWall_Vault_Actor::On_Box_End_Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//Cast the input argument "OtherActor" to the class ATechnical_Animator_Character to retrieve a reference to the character class which just ended overlapping with the "Area_Box".

	ATechnical_Animator_Character* Overlapping_Actor{Cast<ATechnical_Animator_Character>(OtherActor)};

	if(Overlapping_Actor)
	Overlapping_Actor->Set_Overlapping_Wall_Vault_Actor(nullptr);
}

#pragma endregion

#pragma region Helper_Functions

FVector AWall_Vault_Actor::Move_Vector_Up(const FVector& Initial_Location, const float& Move_Value) const
{
	const FVector Up_Vector{GetActorUpVector()};
	const FVector New_Location{Initial_Location + (Up_Vector * Move_Value)};

	return New_Location;
}

FVector AWall_Vault_Actor::Move_Vector_Down(const FVector& Initial_Location, const float& Move_Value) const
{
	const FVector Up_Vector{GetActorUpVector()};
	const FVector New_Location{Initial_Location + (Up_Vector * Move_Value)};

	return New_Location;
}

#pragma endregion

#pragma region Ray_Casts

void AWall_Vault_Actor::Obtain_Wall_Top_Result()
{
	const FVector Wall_Vault_Actor_Location{GetActorLocation()};
	
	const FVector Start{Move_Vector_Up(Wall_Vault_Actor_Location, 110.f)};
	const FVector End{Move_Vector_Down(Start, 15.f)};

	UKismetSystemLibrary::SphereTraceSingleForObjects(
		this,
		Start,
		End,
		5.f,
		Get_Wall_Top_Location_Trace_Types,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		Wall_Vault_Actor_Wall_Top_Result,
		false
	);
}

#pragma endregion