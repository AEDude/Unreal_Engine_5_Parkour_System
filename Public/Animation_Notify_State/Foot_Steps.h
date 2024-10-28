// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Foot_Steps.generated.h"

class UCustom_Movement_Component;

/**
 * 
 */
UCLASS()
class TECHNICAL_ANIMATOR_API UFoot_Steps : public UAnimNotifyState
{
	GENERATED_BODY()

	//Override "NotifyBegin". Depending on the surface that the character is traveling on, the appropriate foot steps will be called.
	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	FVector Move_Vector_Up(UCustom_Movement_Component* Custom_Movement_Component, const FVector& Initial_Location, const float& Move_Value);

	FVector Move_Vector_Down(UCustom_Movement_Component* Custom_Movement_Component, const FVector& Initial_Location, const float& Move_Value);

};
