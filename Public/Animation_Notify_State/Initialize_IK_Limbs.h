// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Initialize_IK_Limbs.generated.h"

/**
 * 
 */
UCLASS()
class TECHNICAL_ANIMATOR_API UInitialize_IK_Limbs : public UAnimNotifyState
{
	GENERATED_BODY()

private:

	//Override "NotifyEnd". This function will call the setter functions located within the UCustom_Movement_Component which handle the execution of 
	//setting the locations and rotations of the hands and feet when the character intitially starts shimmying. These locations and rotations are set
	//by calling the respective "Parkour_Locomotion_Interface" functions passing in the location and rotation obtained by const&. Said rotations and locations
	//are obtained via algoriths which perform ray traces.
	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

};
