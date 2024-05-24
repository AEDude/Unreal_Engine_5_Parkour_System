// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Interfaces/Parkour_Locomotion_Interface.h"
//#include "Gameplay_Tags/Gameplay_Tags.h"
#include "Native_Gameplay_Tags/Native_Gameplay_Tags.h"
#include "Enumarators/Ground_Locomotion_State.h"
#include "Enumarators/Ground_Locomotion_Starting_Direction.h"
#include "Character_Animation_Instance.generated.h"

class ATechnical_Animator_Character;
class UCustom_Movement_Component;
class USkeletalMeshComponent;
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
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativePostEvaluateAnimation() override;

private:
	UPROPERTY()
	ATechnical_Animator_Character* Technical_Animator_Character;
	
	UPROPERTY()
	UCustom_Movement_Component* Custom_Movement_Component;

	UPROPERTY()
	USkeletalMeshComponent* Mesh;

#pragma region Custom_Locomotion_Helper

	void Get_Input_Vector();
	
	void Get_Acceleration();

	void Get_Ground_Speed();

	void Get_Air_Speed();

	void Get_Velocity();

	void Calculate_Direction();

	void Get_Should_Move();

	void Get_Is_Falling();

#pragma endregion

#pragma region Custom_Locomotion_Core

	void Update_Variables_On_Secondary_Thread(const float& DeltaSeconds);
	
	void Find_Ground_Locomotion_State();

	void Idle_Turn_In_Place();

	void Update_Rotation_Turn_In_Place();

	void Track_Ground_Locomotion_State_Idle(const EGround_Locomotion_State& Ground_Locomotion_State_Reference);

	void Track_Ground_Locomotion_State_Walking(const EGround_Locomotion_State& Ground_Locomotion_State_Reference);

	void Track_Ground_Locomotion_State_Jogging(const EGround_Locomotion_State& Ground_Locomotion_State_Reference);
	
	void Find_Locomotion_Start_Direction(const float& Starting_Angle);
	
	void Update_Character_Rotation();

	void Update_Character_Rotation_While_Moving();

	void Update_On_Movement_Enter();

	void Update_Locomotion_Play_Rate();

	void Get_Predicted_Stop_Distance_Variables();

	void Get_Dynamic_Look_Offset_Values(const float& DeltaSeconds);

	void Dynamic_Look_Offset_Weight(const float& DeltaSeconds);

	void Calculate_Dynamic_Lean_Angle();

#pragma endregion

#pragma region Custom_Locomotion_Core_BP_Variables

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	FVector Input_Vector{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	FVector Acceleration{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	float Ground_Speed{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	float Air_Speed{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	FVector Velocity{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	bool bShould_Move{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	bool bIs_Falling{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	EGround_Locomotion_State Ground_Locomotion_State{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	EGround_Locomotion_Starting_Direction Ground_Locomotion_Starting_Direction{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	float Animation_Play_Rate{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	FRotator Turn_In_Place_Starting_Rotation{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	float Turn_In_Place_Delta{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	float Turn_In_Place_Minimum_Threshold{45.f};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	float Turn_In_Place_Target_Angle{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	float Locomotion_Start_Angle{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	bool bCan_Turn_In_Place{false};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	bool bDisable_Turn_In_Place{true};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	bool bTurn_In_Place_Flip_Flop{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	bool bUse_Seperate_Braking_Friction{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	float Braking_Friction{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	float Ground_Friction{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	float Braking_Friction_Factor{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	float Braking_Deceleration_Walking{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	float Direction_For_Orientation_Warping{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	float Distance_To_Match{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	double Left_Right_Look_Value{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	float Dynamic_Look_Weight{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	float Dynamic_Lean_Angle{};

#pragma endregion
	
#pragma region Custom_Locomotion_Core_Variables

	bool Do_Once_1{true};

	bool Do_Once_2{true};

	FRotator Starting_Rotation{};

	FRotator Primary_Rotation{};

	FRotator Secondary_Rotation{};
	

	bool Do_Once_3{true};

	bool Do_Once_4{true};
	
	FRotator Target_Input_Rotation{};

	FVector Current_Input_Vector{};

    FRotator Current_Input_Rotation{};

	
	
	
	FRotator Target_Input_Rotation_1{};

	FRotator Target_Input_Rotation_2{};

	FRotator Target_Input_Rotation_3{};

	FRotator Target_Input_Rotation_4{};

	FRotator Target_Input_Rotation_5{};

	FRotator Target_Input_Rotation_5_5{};

	FRotator Target_Input_Rotation_7{};

	
	
	FVector Interpolated_Direction{};

	FVector Interpolated_Direction_1{};

	FVector Interpolated_Direction_2{};

	FVector Interpolated_Direction_3{};

	FVector Interpolated_Direction_4{};

	FVector Interpolated_Direction_5{};

	FVector Interpolated_Direction_5_5{};

	FVector Interpolated_Direction_7{};

	double Look_At_Value_Final_Interpolation{};

	bool bLook_Left_Right_Debug_Visibility{true};

	float Character_Yaw{};

#pragma endregion
	

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


#pragma region Parkour_Locomotion

	/*Used to set new Parkour State within the Animation Blueprint in the editor. This line tells this animation instance class
	that this function can both be called and overriden from Blueprints.*/
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Parkour_State(const FGameplayTag& New_Parkour_State);

	/*This line tells the animation instance class that is has a function of this name and signature to inherit from the the 
	interface class declared above "public IParkour_Locomotion_Interface". This is how call to the interface functions are able 
	to interact with this class.*/
	virtual void Set_Parkour_State_Implementation(const FGameplayTag& New_Parkour_State) override;


	/*Used to set new Parkour State within the Animation Blueprint in the editor. This line tells this animation instance class
	that this function can both be called and overriden from Blueprints.*/
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Parkour_Action(const FGameplayTag& New_Parkour_Action);

	/*This line tells the animation instance class that is has a function of this name and signature to inherit from the the 
	interface class declared above "public IParkour_Locomotion_Interface". This is how call to the interface functions are able 
	to interact with this class.*/
	virtual void Set_Parkour_Action_Implementation(const FGameplayTag& New_Parkour_Action) override;


	/*Used to set new Parkour State within the Animation Blueprint in the editor. This line tells this animation instance class
	that this function can both be called and overriden from Blueprints.*/
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Parkour_Climb_Style(const FGameplayTag& New_Climb_Style);

	/*This line tells the animation instance class that is has a function of this name and signature to inherit from the the 
	interface class declared above "public IParkour_Locomotion_Interface". This is how call to the interface functions are able 
	to interact with this class.*/
	virtual void Set_Parkour_Climb_Style_Implementation(const FGameplayTag& New_Climb_Style) override;
    

	/*Used to set new Parkour State within the Animation Blueprint in the editor. This line tells this animation instance class
	that this function can both be called and overriden from Blueprints.*/
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Parkour_Wall_Run_Side(const FGameplayTag& New_Wall_Run_Side);

	/*This line tells the animation instance class that is has a function of this name and signature to inherit from the the 
	interface class declared above "public IParkour_Locomotion_Interface". This is how call to the interface functions are able 
	to interact with this class.*/
	virtual void Set_Parkour_Wall_Run_Side_Implementation(const FGameplayTag& New_Wall_Run_Side) override;
    

	/*Used to set new Parkour State within the Animation Blueprint in the editor. This line tells this animation instance class
	that this function can both be called and overriden from Blueprints.*/
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Parkour_Direction(const FGameplayTag& New_Climb_Direction);

	/*This line tells the animation instance class that is has a function of this name and signature to inherit from the the 
	interface class declared above "public IParkour_Locomotion_Interface". This is how calls to the interface functions are able 
	to interact with this class.*/
	virtual void Set_Parkour_Direction_Implementation(const FGameplayTag& New_Climb_Direction) override;

	#pragma endregion


#pragma region Limbs_Location_And_Rotations


#pragma region Left_Limbs

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parkour_Limbs", meta = (AllowPrivateAccess ="true"))
	FVector Left_Hand_Shimmy_Location{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parkour_Limbs", meta = (AllowPrivateAccess = "true"))
	FRotator Left_Hand_Shimmy_Rotation{};
	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parkour_Limbs", meta = (AllowPrivateAccess = "true"))
	FVector Left_Foot_Shimmy_Location{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parkour_Limbs", meta = (AllowPrivateAccess = "true"))
	FRotator Left_Foot_Shimmy_Rotation{};



	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Left_Hand_Shimmy_Location(const FVector& New_Left_Hand_Shimmy_Location);

	virtual void Set_Left_Hand_Shimmy_Location_Implementation(const FVector& New_Left_Hand_Shimmy_Location) override;


    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Left_Hand_Shimmy_Rotation(const FRotator& New_Left_Hand_Rotation);

	virtual void Set_Left_Hand_Shimmy_Rotation_Implementation(const FRotator& New_Left_Hand_Shimmy_Rotation) override;



    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Left_Foot_Shimmy_Location(const FVector& New_Left_Foot_Location);

	virtual void Set_Left_Foot_Shimmy_Location_Implementation(const FVector& New_Left_Foot_Shimmy_Location) override;



    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Left_Foot_Shimmy_Rotation(const FRotator& New_Left_Foot_Rotation);

	virtual void Set_Left_Foot_Shimmy_Rotation_Implementation(const FRotator& New_Left_Foot_Shimmy_Rotation) override;

#pragma endregion


#pragma region Right_Limbs

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parkour_Limbs", meta = (AllowPrivateAccess = "true"))
	FVector Right_Hand_Shimmy_Location{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parkour_Limbs", meta = (AllowPrivateAccess = "true"))
	FRotator Right_Hand_Shimmy_Rotation{};
	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parkour_Limbs", meta = (AllowPrivateAccess = "true"))
	FVector Right_Foot_Shimmy_Location{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parkour_Limbs", meta = (AllowPrivateAccess = "true"))
	FRotator Right_Foot_Shimmy_Rotation{};



    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Right_Hand_Shimmy_Location(const FVector& New_Right_Hand_Location);

	virtual void Set_Right_Hand_Shimmy_Location_Implementation(const FVector& New_Right_Hand_Shimmy_Location) override;



    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Right_Hand_Shimmy_Rotation(const FRotator& New_Right_Hand_Rotation);

	virtual void Set_Right_Hand_Shimmy_Rotation_Implementation(const FRotator& New_Right_Hand_Shimmy_Rotation) override;



    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Right_Foot_Shimmy_Location(const FVector& New_Right_Foot_Location);

	virtual void Set_Right_Foot_Shimmy_Location_Implementation(const FVector& New_Right_Foot_Shimmy_Location) override;



    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Right_Foot_Shimmy_Rotation(const FRotator& New_Right_Foot_Rotation);

	virtual void Set_Right_Foot_Shimmy_Rotation_Implementation(const FRotator& New_Right_Foot_Shimmy_Rotation) override;

#pragma endregion


#pragma endregion


#pragma endregion


#pragma region Parkour_Gameplay_Tags

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parkour_Locomotion", meta = (AllowPrivateAccess = "true"))
	FGameplayTag Parkour_State{FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Free.Roam")))};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parkour_Locomotion", meta = (AllowPrivateAccess = "true"))
	FGameplayTag Parkour_Action{FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.No.Action")))};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parkour_Locomotion", meta = (AllowPrivateAccess = "true"))
	FGameplayTag Parkour_Climb_Style{FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Climb.Style.None")))};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parkour_Locomotion", meta = (AllowPrivateAccess = "true"))
	FGameplayTag Parkour_Wall_Run_Side{FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Wall.Run.Side.None")))};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parkour_Locomotion", meta = (AllowPrivateAccess = "true"))
	FGameplayTag Parkour_Direction{FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.None")))};
	
#pragma endregion


#pragma region Others

	UPROPERTY(BlueprintReadOnly, Category = "Parkour_Shimmying", meta = (AllowPrivateAccess = "true"))
	double Forward_Backward_Movement_Value{};

	UPROPERTY(BlueprintReadOnly, Category = "Parkour_Shimmying", meta = (AllowPrivateAccess = "true"))
	double Right_Left_Movement_Value{};

	UPROPERTY(BlueprintReadOnly, Category = "Parkour_Shimmying", meta = (AllowPrivateAccess = "true"))
	float Left_Hand_Curve_Alpha{};

	UPROPERTY(BlueprintReadOnly, Category = "Parkour_Shimmying", meta = (AllowPrivateAccess = "true"))
	float Right_Hand_Curve_Alpha{};

#pragma endregion


#pragma endregion


public:
	
	FORCEINLINE float Anim_Get_Air_Speed() const {return Air_Speed;}
	FORCEINLINE float Anim_Get_Ground_Speed() const {return Ground_Speed;}
	FORCEINLINE void Set_bLook_Left_Right_Debug_Visibility(const bool& bVisible) {bLook_Left_Right_Debug_Visibility = bVisible;}



};	