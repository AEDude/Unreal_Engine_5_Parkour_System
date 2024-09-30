// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Stairs_Actor.generated.h"


class UBoxComponent;
class ATechnical_Animator_Character;


UCLASS()
class TECHNICAL_ANIMATOR_API AStairs_Actor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStairs_Actor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


#pragma region delegate callbacks

	UFUNCTION()
	void On_Box_Begin_Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void On_Box_End_Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

#pragma region endregion


#pragma region BP_Variables

UPROPERTY(VisibleAnywhere, Category = "Stairs")
UBoxComponent* Area_Box{};

#pragma region endregion



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
