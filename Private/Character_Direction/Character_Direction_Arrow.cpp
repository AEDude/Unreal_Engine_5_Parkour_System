// Fill out your copyright notice in the Description page of Project Settings.


#include "Character_Direction/Character_Direction_Arrow.h"
#include "Components/ArrowComponent.h"

// Sets default values
ACharacter_Direction_Arrow::ACharacter_Direction_Arrow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//Create the scene component which will be the root component.
	Default_Scene_Root = CreateDefaultSubobject<USceneComponent>(TEXT("Default_Scene_Root"));
	SetRootComponent(Default_Scene_Root);
	
	//Create the arrow component which will be attached to the root component.
	Character_Direction_Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Character_Direction_Arrow"));
	Character_Direction_Arrow->SetupAttachment(RootComponent);
	Character_Direction_Arrow->SetRelativeRotation(FRotator(FRotator::TRotator(0.f, 0.f, 0.f)));
	Character_Direction_Arrow->SetArrowFColor(FColor::White);
	Character_Direction_Arrow->bHiddenInGame = false;
}

// Called when the game starts or when spawned
void ACharacter_Direction_Arrow::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACharacter_Direction_Arrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

