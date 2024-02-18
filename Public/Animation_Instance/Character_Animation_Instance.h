// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Interfaces/Parkour_Locomotion_Interface.h"
#include "Gameplay_Tags/Gameplay_Tags.h"
#include "Character_Animation_Instance.generated.h"

class ATechnical_AnimatorCharacter;
class UCustom_Movement_Component;
/**
 * 
 */
UCLASS()																		   /*Inherits functions from the this custom 
																				   interface class*/
class TECHNICAL_ANIMATOR_API UCharacter_Animation_Instance : public UAnimInstance, public IParkour_Locomotion_Interface
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Reference, meta = (AllowPrivateAccess = "true"))
	float Ground_Speed;
	void Get_Ground_Speed();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Reference, meta = (AllowPrivateAccess = "true"))
	float Air_Speed;
	void Get_Air_Speed();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Reference, meta = (AllowPrivateAccess = "true"))
	bool bShould_Move;
	void Get_Should_Move();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Reference, meta = (AllowPrivateAccess = "true"))
	bool bIs_Falling;
	void Get_Is_Falling();

	#pragma region Climbing

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Reference, meta = (AllowPrivateAccess = "true"))
	bool bIs_Climbing;
	void Get_Is_Climbing();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Reference, meta = (AllowPrivateAccess = "true"))
	FVector Climb_Velocity;
	void Get_Climb_Velocity();

	#pragma endregion

	#pragma region Take_Cover

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Reference, meta = (AllowPrivateAccess = "true"))
	bool bIs_Taking_Cover;
	void Get_Is_Taking_Cover();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Reference, meta = (AllowPrivateAccess = "true"))
	FVector Take_Cover_Velocity;
	void Get_Take_Cover_Velocity();

	#pragma endregion

protected:

	#pragma region Parkour

	#pragma region Parkour_Interface

	/*Used to set new Parkour State within the Animation Blueprint in the editor. This line tells this animation instance class
	that this function can both be called and overriden from Blueprints.*/
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool Set_Parkour_State(const FGameplayTag& New_Parkour_State);

	/*This line tells the animation instance class that is has a function of this name and signature to inherit from the the 
	interface class declared above "public IParkour_Locomotion_Interface". This is how call to the interface functions are able 
	to interact with this class.*/
	virtual bool Set_Parkour_State_Implementation(const FGameplayTag& New_Parkour_State) override;


	/*Used to set new Parkour State within the Animation Blueprint in the editor. This line tells this animation instance class
	that this function can both be called and overriden from Blueprints.*/
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool Set_Parkour_Action(const FGameplayTag& New_Parkour_Action);

	/*This line tells the animation instance class that is has a function of this name and signature to inherit from the the 
	interface class declared above "public IParkour_Locomotion_Interface". This is how call to the interface functions are able 
	to interact with this class.*/
	virtual bool Set_Parkour_Action_Implementation(const FGameplayTag& New_Parkour_Action) override;


	/*Used to set new Parkour State within the Animation Blueprint in the editor. This line tells this animation instance class
	that this function can both be called and overriden from Blueprints.*/
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool Set_Climb_Style(const FGameplayTag& New_Climb_Style);

	/*This line tells the animation instance class that is has a function of this name and signature to inherit from the the 
	interface class declared above "public IParkour_Locomotion_Interface". This is how call to the interface functions are able 
	to interact with this class.*/
	virtual bool Set_Climb_Style_Implementation(const FGameplayTag& New_Climb_Style) override;
    

	/*Used to set new Parkour State within the Animation Blueprint in the editor. This line tells this animation instance class
	that this function can both be called and overriden from Blueprints.*/
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool Set_Climb_Direction(const FGameplayTag& New_Climb_Direction);

	/*This line tells the animation instance class that is has a function of this name and signature to inherit from the the 
	interface class declared above "public IParkour_Locomotion_Interface". This is how call to the interface functions are able 
	to interact with this class.*/
	virtual bool Set_Climb_Direction_Implementation(const FGameplayTag& New_Climb_Direction) override;

	#pragma endregion

	#pragma region Parkour_Gameplay_Tags

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Parkour, meta = (AllowPrivateAccess = "true"))
	FGameplayTag Parkour_State{FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Free.Roam")))};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Parkour, meta = (AllowPrivateAccess = "true"))
	FGameplayTag Parkour_Action{FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.No.Action")))};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Parkour, meta = (AllowPrivateAccess = "true"))
	FGameplayTag Climb_Direction{FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.None")))};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Parkour, meta = (AllowPrivateAccess = "true"))
	FGameplayTag Climb_Style{FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Climb.Style.Braced.Climb")))};

	#pragma endregion


	#pragma endregion

};	