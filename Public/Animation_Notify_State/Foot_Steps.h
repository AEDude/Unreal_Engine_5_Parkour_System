// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Foot_Steps.generated.h"

class ATechnical_Animator_Character;
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

	FVector Move_Vector_Up(const FVector& Initial_Location, const float& Move_Value);

	FVector Move_Vector_Down(const FVector& Initial_Location, const float& Move_Value);

	EPhysicalSurface Surface_Type{};

	TArray<TEnumAsByte<EObjectTypeQuery>> Object_Trace_Types{};

	UPROPERTY()
	ATechnical_Animator_Character* Technical_Animator_Character{};

	UPROPERTY()
	UCustom_Movement_Component* Custom_Movement_Component{};
	
};
