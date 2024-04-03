// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Actor_Movement_Component.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TECHNICAL_ANIMATOR_API UActor_Movement_Component : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActor_Movement_Component();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	
	void Move_Component(const float& DeltaTime);
	
	UPROPERTY()
	AActor* Component_Owner{};
	
	FVector Component_Starting_Location{};

	UPROPERTY(EditAnywhere, Category = "Translation")
	FVector Component_Velocity{FVector (0, 0, 0)};

	FVector Move{};

	FVector Component_Current_Location{};

	UPROPERTY(EditAnywhere, Category = "Translation")
	double Maximum_Move_Distance{};


	void Rotate_Component(const float& DeltaTime);

	UPROPERTY(EditAnywhere, Category = "Rotation")
	FRotator Component_Rotation{};	
};
