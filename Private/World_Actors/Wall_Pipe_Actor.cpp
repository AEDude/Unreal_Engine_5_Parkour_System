// Fill out your copyright notice in the Description page of Project Settings.


#include "World_Actors/Wall_Pipe_Actor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"



// Sets default values
AWall_Pipe_Actor::AWall_Pipe_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWall_Pipe_Actor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWall_Pipe_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Generate_Custom_Wall_Pipe_Actor_Forward_Vector();
}


FVector AWall_Pipe_Actor::Move_Vector_Forward(const FVector& Initial_Location, const FRotator& Rotation, const float& Move_Value)
{
	const FVector Move_Direction{UKismetMathLibrary::GetForwardVector(Rotation)};
	const FVector Destination{Initial_Location + (Move_Direction * Move_Value)};

	return Destination;
}


void AWall_Pipe_Actor::Generate_Custom_Wall_Pipe_Actor_Forward_Vector()
{
	const FVector Actor_Location{GetActorLocation()};
	const FVector Actor_Forward_Vector{GetActorForwardVector()};
	const FRotator Actor_Rotation{GetActorRotation()};

	const FVector Start{Actor_Location};
	const FVector End{Move_Vector_Forward(Start, Actor_Rotation, 70.f)};
	
	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		Start,
		End,
		Wall_Pipe_Forward_Vector_Trace_Types,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		Custom_Wall_Pipe_Actor_Forward_Vector_Hit_Result,
		false
	);
	
}

