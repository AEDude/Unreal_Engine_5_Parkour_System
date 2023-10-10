// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character_Animation_Instance.generated.h"


class ATechnical_AnimatorCharacter;
class UCustom_Movement_Component;
/**
 * 
 */
UCLASS()
class TECHNICAL_ANIMATOR_API UCharacter_Animation_Instance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY()
	ATechnical_AnimatorCharacter* Climbing_System_Character;
	
	UPROPERTY()
	UCustom_Movement_Component* Custom_Movement_Component;

	UPROPERTY(VisibleANywhere, BlueprintReadOnly, Category = Reference, meta = (AllowPrivateAccess = "true"))
	float Ground_Speed;
	void Get_Ground_Speed();

	UPROPERTY(VisibleANywhere, BlueprintReadOnly, Category = Reference, meta = (AllowPrivateAccess = "true"))
	float Air_Speed;
	void Get_Air_Speed();

	UPROPERTY(VisibleANywhere, BlueprintReadOnly, Category = Reference, meta = (AllowPrivateAccess = "true"))
	bool bShould_Move;
	void Get_Should_Move();

	UPROPERTY(VisibleANywhere, BlueprintReadOnly, Category = Reference, meta = (AllowPrivateAccess = "true"))
	bool bIs_Falling;
	void Get_Is_Falling();

	UPROPERTY(VisibleANywhere, BlueprintReadOnly, Category = Reference, meta = (AllowPrivateAccess = "true"))
	bool bIs_Climbing;
	void Get_Is_Climbing();

	UPROPERTY(VisibleANywhere, BlueprintReadOnly, Category = Reference, meta = (AllowPrivateAccess = "true"))
	FVector Climb_Velocity;
	void Get_Climb_Velocity();
};	


