// Fill out your copyright notice in the Description page of Project Settings.


#include "World_Actor_Components/Actor_Movement_Component.h"

// Sets default values for this component's properties
UActor_Movement_Component::UActor_Movement_Component()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UActor_Movement_Component::BeginPlay()
{
	Super::BeginPlay();

	// ...

	Component_Owner = Cast<AActor>(GetOwner());
	
	Component_Starting_Location = Component_Owner->GetActorLocation();
	
}


// Called every frame
void UActor_Movement_Component::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	Move_Component(DeltaTime);

	Rotate_Component(DeltaTime);
}

void UActor_Movement_Component::Move_Component(const float& DeltaTime)
{
	//Get the components current location.
	Component_Current_Location = Component_Owner->GetActorLocation();
	//Move Component in desired direction
	Component_Current_Location = Component_Current_Location + (Component_Velocity * DeltaTime);
	Component_Owner->SetActorLocation(Component_Current_Location);
	

	//Get the distance moved from the initial location
	const double Distance_Moved{FVector::Dist(Component_Starting_Location,  Component_Current_Location)};

	//If distance moved is greater than the desired maximum move distance move the component back.
	if(Distance_Moved >= Maximum_Move_Distance)
	{
		/*The Component may have overshot the maximum move distance, so move it to the location of where it would be at the maximum move distance.*/
		
		//Get the normal of the component's velocity
		FVector Components_Movement_Normal{Component_Velocity.GetSafeNormal()};
		//Move the component back to the location of where it would be at the maximum move distance.
		Component_Starting_Location = Component_Starting_Location + (Components_Movement_Normal * Maximum_Move_Distance);
		//Set the location of the component
		Component_Owner->SetActorLocation(Component_Starting_Location);
		//Reverse the velocity of the component.
		Component_Velocity = -Component_Velocity;
	}
}

void UActor_Movement_Component::Rotate_Component(const float& DeltaTime)
{
	Component_Owner->AddActorLocalRotation(Component_Rotation * DeltaTime);
}

