// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Character_Direction_Arrow.generated.h"

class USceneComponent;
class UArrowComponent;

UCLASS()
class TECHNICAL_ANIMATOR_API ACharacter_Direction_Arrow : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACharacter_Direction_Arrow();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UPROPERTY()
	USceneComponent* Default_Scene_Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UArrowComponent* Character_Direction_Arrow;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
