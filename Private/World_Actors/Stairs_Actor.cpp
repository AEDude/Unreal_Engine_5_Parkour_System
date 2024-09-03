// Fill out your copyright notice in the Description page of Project Settings.


#include "World_Actors/Stairs_Actor.h"

// Sets default values
AStairs_Actor::AStairs_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AStairs_Actor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStairs_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

