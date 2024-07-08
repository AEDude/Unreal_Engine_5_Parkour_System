// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
//#include "Gameplay_Tags/Gameplay_Tags.h"
#include "Native_Gameplay_Tags/Native_Gameplay_Tags.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Custom_Movement_Component.generated.h"

DECLARE_DELEGATE(F_On_Enter_Climb_State)
DECLARE_DELEGATE(F_On_Exit_Climb_State)
DECLARE_DELEGATE(F_On_Enter_Take_Cover_State)
DECLARE_DELEGATE(F_On_Exit_Take_Cover_State)

class UCharacterMovementComponent;
class UCapsuleComponent;
class ATechnical_Animator_Character;
class USkeletalMeshComponent;
class UMotionWarpingComponent;
class UAnimInstance;
class UAnimMontage;
class UCameraComponent;
class ACharacter_Direction_Arrow;
class IParkour_Locomotion_Interface;
class APlayer_Controller;
class UParkour_Action_Data;
class AWall_Pipe_Actor;
class ABalance_Traversal_Actor;

UENUM(BlueprintType)
namespace E_Custom_Movement_Mode
{
	enum Type
	{
		MOVE_Climb UMETA(DisplayName = "Climb Mode"),
		MOVE_Take_Cover UMETA(DisplayName = "Take Cover Mode")
	};
}

/**
 * 
 */
UCLASS()
class TECHNICAL_ANIMATOR_API UCustom_Movement_Component : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:

	UCustom_Movement_Component();

	F_On_Enter_Climb_State On_Enter_Climb_State_Delegate;

	F_On_Exit_Climb_State On_Exit_Climb_State_Delegate;

	F_On_Enter_Take_Cover_State On_Enter_Take_Cover_State_Delegate;

	F_On_Exit_Take_Cover_State On_Exit_Take_Cover_State_Delegate;


protected:

#pragma region Overriden_Functions
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxAcceleration() const override;
	virtual FVector ConstrainAnimRootMotionVelocity(const FVector& RootMotionVelocity, const FVector& CurrentVelocity) const override;

#pragma endregion

private:

	UPROPERTY()
	UAnimInstance* Owning_Player_Animation_Instance{};

	UPROPERTY()
	ATechnical_Animator_Character* Owning_Player_Character{};

#pragma region Climb_Region

#pragma region Climb_Traces

	TArray<FHitResult> DoCapsuleTraceMultiByObject(const FVector& Start, const FVector& End, bool B_Show_Debug_Shape = false, bool bDrawPersistantShapes = false);

	FHitResult Do_Line_Trace_Single_By_Object(const FVector& Start, const FVector& End, bool B_Show_Debug_Shape = false, bool bDrawPersistantShapes = false);
#pragma endregion

#pragma region Climb_Core

	bool Trace_Climbable_Surfaces();

	FHitResult Trace_From_Eye_Height(float Trace_Distance, float Trace_Start_Offset = 0.f, bool B_Show_Debug_Shape = false, bool bDrawPersistantShapes = false);

	bool Can_Start_Climbing();

	bool Can_Climb_Down_Ledge();

	void Start_Climbing();

	void Stop_Climbing();

	void Physics_Climb(float deltaTime, int32 Iterations);

	void Process_Climbable_Surface_Info();

	bool Check_Should_Stop_Climbing();

	bool Check_Has_Reached_Floor();

	FQuat Get_Climb_Rotation(float DeltaTime);
	
	void Snap_Movement_To_Climbable_Surfaces(float DeltaTime);

	bool Check_Has_Has_Reached_Ledge();

	void Try_Start_Vaulting();

	bool Can_Start_Vaulting(FVector& Out_Vault_Start_Position, FVector& Out_Vault_Land_Position);

	void Play_Climbing_Montage(UAnimMontage* MontageToPlay);

	UFUNCTION()
	void On_Climbing_Montage_Ended(UAnimMontage* Montage, bool bInterrupted);

	void Set_Motion_Warping_Target(const FName& In_Warping_Target_Name, const FVector& In_Target_Position);

	void Handle_Hop_Hop();
	
	bool bCheck_Can_Hop_Up(FVector& Out_Hop_Up_Target_Point);

	void Handle_Hop_Down();
	
	bool bCheck_Can_Hop_Down(FVector& Out_Hop_Down_Target_Point);

	void Handle_Hop_Left();
	
	bool bCheck_Can_Hop_Left(FVector& Out_Hop_Left_Target_Point);

	void Handle_Hop_Right();
	
	bool bCheck_Can_Hop_Right(FVector& Out_Hop_Right_Target_Point);

#pragma endregion

#pragma region Climb_Core_Variables

	TArray<FHitResult> Climable_Surfaces_Traced_Results;

	FVector Current_Climbable_Surface_Location;

	FVector Current_Climbable_Surface_Normal;

#pragma endregion

#pragma region Climb_BP_Variables
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery> > Climable_Surface_Trace_Types;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	float Climb_Capsule_Trace_Radius = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	float Climb_Capsule_Trace_Half_Height = 72.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	float Max_Break_Climb_Deceleration = 400.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	float Max_Climb_Speed = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	float Max_Climb_Acceleration = 300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	float Climb_Down_Walkable_Surface_Trace_Offset = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	float Climb_Down_Ledge_Trace_Offset = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	float Climbing_Hop_Trace_Offset = 170.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Idle_To_Climb_Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Climb_To_Top_Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Climb_Down_Ledge_Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Vaulting_Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Hop_Up_Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Hop_Down_Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Hop_Left_Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Hop_Right_Montage;

#pragma endregion

#pragma endregion



#pragma region Take_Cover_Region

#pragma region Take_Cover_Traces

	TArray<FHitResult> Do_Capsule_Trace_Multi_By_Object_Take_Cover(const FVector& Start, const FVector& End, bool B_Show_Debug_Shape = false,  bool B_Draw_Persistant_Shapes = false);

	FHitResult Do_Line_Trace_Single_By_Object_Take_Cover(const FVector& Start, const FVector& End, bool B_Show_Debug_Shape = false, bool B_Draw_Persistent_Shapes = false);

	FHitResult Do_Sphere_Trace_For_Objects(const FVector& Start, const FVector& End, bool B_Show_Debug_Shape = false, bool B_Draw_Persistent_Shapes = false);

#pragma endregion

#pragma region Take_Cover_Core

	bool Capsule_Trace_Take_Cover_Surfaces();

	bool Capsule_Trace_Ground_Surface();

	FHitResult Sphere_Trace_Trace_Take_Cover();

	FHitResult Line_Trace_Check_Cover_Right(float Trace_Distance, float Trace_Start_Offset = 55.f);

	FHitResult Line_Trace_Check_Cover_Left(float Trace_Distance, float Trace_Start_Offset = 55.f);

	bool Can_Take_Cover(FVector& Out_Take_Cover_End_Position);

	void Start_Take_Cover();

	void Stop_Take_Cover();

	void Physics_Take_Cover(float deltaTime, int32 Iterations);

	void Process_Take_Cover_Surface_Info();

	void Process_Take_Cover_Ground_Surface_Info();

	bool Check_Should_Exit_Take_Cover();

	FQuat Get_Take_Cover_Rotation(float DeltaTime);

	void Snap_Movement_To_Take_Cover_Surfaces(float DeltaTime);

	void Take_Cover_Snap_Movement_To_Ground(float DeltaTime);

	void Check_Has_Reached_Take_Cover_Edge();

	void Play_Take_Cover_Montage(UAnimMontage* Montage_To_Play);

	UFUNCTION()
	void On_Take_Cover_Montage_Ended(UAnimMontage* Montage, bool bInterrupted);

#pragma endregion

#pragma region Take_Cover_Variables

	TArray<FHitResult> Take_Cover_Surfaces_Traced_Results;

	TArray<FHitResult> Take_Cover_Ground_Surface_Traced_Results;

	FHitResult Sphere_Trace_Hit_Result;

	FVector Current_Take_Cover_Surface_Location;

	FVector Current_Take_Cover_Surface_Normal;

	FVector Current_Take_Cover_Ground_Surface_Location;

	FVector Current_Take_Cover_Ground_Surface_Normal;

#pragma endregion

#pragma region Take_Cover_BP_Variables

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Take Cover", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery> > Take_Cover_Surface_Trace_Types;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Take Cover", meta = (AllowPrivateAccess = "true"))
	float Take_Cover_Capsule_Trace_Radius = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Take Cover", meta = (AllowPrivateAccess = "true"))
	float Take_Cover_Sphere_Trace_Radius = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Take Cover", meta = (AllowPrivateAccess = "true"))
	float Take_Cover_Capsule_Trace_Half_Height = 70.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Take Cover", meta = (AllowPrivateAccess = "true"))
	float Take_Cover_Check_Cover_Edge = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Take Cover", meta = (AllowPrivateAccess = "true"))
	float Max_Brake_Take_Cover_Deceleration = 3000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Take Cover", meta = (AllowPrivateAccess = "true"))
	float Max_Take_Cover_Speed = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Take Cover", meta = (AllowPrivateAccess = "true"))
	float Max_Take_Cover_Acceleration = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Take Cover", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Idle_To_Take_Cover_Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Take Cover", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Exit_Cover_To_Stand;

#pragma endregion

#pragma endregion



#pragma region Parkour_Region

#pragma region Parkour_Helper

	FVector Move_Vector_Up(const FVector& Initial_Location, const float& Move_Value) const;

	FVector Move_Vector_Down(const FVector& Initial_Location, const float& Move_Value) const;

	FVector Move_Vector_Left(const FVector& Initial_Location, const FRotator& Rotation, const float& Move_Value) const;

	FVector Move_Vector_Right(const FVector& Initial_Location, const FRotator& Rotation, const float& Move_Value) const;

	FVector Move_Vector_Forward(const FVector& Initial_Location, const FRotator& Rotation, const float& Move_Value) const;

	FVector Move_Vector_Backward(const FVector& Initial_Location, const FRotator& Rotation, const float& Move_Value) const;

	FRotator Add_Rotator(const FRotator& Initial_Rotation, const float& Add_To_Rotation) const;

	FRotator Reverse_Wall_Normal_Rotation_Z(const FVector& Initial_Wall_Normal) const;

	void Draw_Debug_Sphere(const FVector& Location, const float& Radius, const FColor& Color, const float& Duration, const bool& bDraw_Debug_Shape_Persistent, const float& Lifetime) const;

#pragma endregion

#pragma region Parkour_Pointers

	UPROPERTY()
	TArray<AActor*> Actors_To_Ignore{};
	
	UPROPERTY()
	ACharacter_Direction_Arrow* Character_Direction_Arrow{};

	UPROPERTY()
	UCharacterMovementComponent* Character_Movement{};

	UPROPERTY()
	USkeletalMeshComponent* Mesh{};

	UPROPERTY()
	UCapsuleComponent* Capsule_Component{};

	UPROPERTY()
	UAnimInstance* Anim_Instance{};

	UPROPERTY()
	UMotionWarpingComponent* Motion_Warping_Component{};

	UPROPERTY()
	UCameraComponent* Camera_Component{};

	UPROPERTY()
	APlayer_Controller* Player_Controller{};
	
	IParkour_Locomotion_Interface* Parkour_Interface{};

	UPROPERTY()
	UParkour_Action_Data* Parkour_Data_Asset{};

	UPROPERTY()
	AWall_Pipe_Actor* Wall_Pipe_Actor{};

	UPROPERTY()
	ABalance_Traversal_Actor* Balance_Traversal_Actor{};

#pragma endregion

#pragma region Initialize_Parkour

	void Attach_Arrow_Actor_To_Character(ATechnical_Animator_Character* Character);

	void Get_Pointer_To_Parkour_Locomotion_Interface_Class();

	//void Get_Pointer_To_Parkour_Action_Data_Class();

	void Initialize_Parkour_Data_Assets_Arrays();

	void Initialize_Actors_To_Ignore_Arrays();

#pragma endregion

#pragma region Parkour_Traces

	void Parkour_Detect_Wall();

	void Grid_Scan_For_Hit_Results(const FVector& Base_Location, const FRotator& Previous_Trace_Rotation, const int& Scan_Width_Value, const int& Scan_Height_Value);

	void Analyze_Grid_Scan_For_Front_Wall_Top_Edge_Hits();

	void Realize_Front_Wall_Top_Edge_Best_Hit();

	void Analyze_Wall_Top_Surface();

	void Calculate_Wall_Top_Surface();

	void Calculate_Wall_Vault_Location();

	double Calculate_Wall_Height();
	
	double Calculate_Wall_Depth();
	
	double Calculate_Vault_Height();

	void Validate_bIs_On_Ground();

	void Decide_Climb_Style(const FVector& Impact_Point, const FRotator& Direction_For_Character_To_Face);

	bool Parkour_Climb_State_Detect_Wall(FHitResult& Parkour_Climbing_Detect_Wall_Hit_Result, FHitResult& Parkour_Climbing_Wall_Top_Result);

	bool Parkour_Climb_State_Are_There_Obstacles_On_Sides_Of_Hands(const FVector& Starting_Impact_Point);

	bool Parkour_Climb_State_Are_There_Obstacles_On_Sides_Of_Body(const FVector& Movemement_Impact_Location) const;

	void Parkour_Climb_Initialize_IK_Hands(const bool& bIs_Left_Hand);

	void Parkour_Climb_Dynamic_IK_Hands(const bool& bIs_Left_Hand);

	void Parkour_Climb_Initialize_IK_Feet(const bool& bIs_Left_Foot);

	void Parkour_Climb_Dynamic_IK_Feet(const bool& bIs_Left_Foot);

	bool Validate_Out_Corner_Shimmying();

	bool Validate_In_Corner_Shimmying();

	bool Validate_Shimmy_180_Shimmy();

	bool Validate_Can_Mantle() const;

	void Hop_Grid_Scan_For_Hop_Hit_Result(const FVector& Previous_Trace_Location, const FRotator& Previous_Trace_Rotation, const int& Scan_Width_Value, const int& Scan_Height_Value);

	void Analyze_Hop_Grid_Scan_For_Front_Wall_Top_Edge_Hits();

	void Realize_Front_Wall_Top_Edge_Best_Hop_Hit();

	bool Get_Hop_Top_Result();

	bool Validate_Absence_Of_Obstacles_Before_Hopping();

	bool Validate_Can_Fly_Hanging_Jump() const;

	bool Validate_Can_Jump_From_Braced_Climb(const bool& bJump_Forward) const;

	bool Validate_Can_Jump_From_Free_Hang() const;

	bool Validate_Drop_To_Shimmy(const int& Maximum_Distance_To_Check_For_Drop);

	void Realize_Wall_Run_Surfaces(const bool& bAnalyze_Characters_Left_Side);

	void Assign_Wall_Run_Hit_Result(const FHitResult& Wall_Found_On_Left_Side, const FHitResult& Wall_Found_On_Right_Side);

	bool Analyze_And_Validate_Wall_Run_Surface();

	void Wall_Run_Detect_Wall(const bool& bAnalyze_Characters_Left_Side);
	
	bool Validate_Can_Jump_From_Wall_Run() const;

	bool Validate_Drop_Off_Ledge_While_Sprinting();

	bool Realize_Wall_Pipe_Surfaces();

	void Parkour_Wall_Pipe_Climb_Initialize_IK_Hands(const bool& bIs_Left_Hand);

	void Parkour_Wall_Pipe_Climb_Dynamic_IK_Hands(const bool& bIs_Left_Hand);

	void Parkour_Wall_Pipe_Climb_Initialize_IK_Feet(const bool& bIs_Left_Foot);

	void Parkour_Wall_Pipe_Climb_Dynamic_IK_Feet(const bool& bIs_Left_Foot);

	bool Parkour_Wall_Pipe_Climb_State_Detect_Wall_Pipe(FHitResult& Parkour_Pipe_Climbing_Wall_Top_Result);

	bool Parkour_Wall_Pipe_Climb_State_Are_There_Obstacles_Ontop_Or_Below_Body(const FVector& Movemement_Impact_Location) const;

	bool Parkour_Wall_Pipe_Climb_Detect_End_Of_Wall_Pipe(const FVector& Movemement_Impact_Location) const;

	bool Validate_Can_Maneuver_To_Free_Hang();

	bool Validate_Wall_Pipe_Can_Mantle();

	bool Validate_Wall_Pipe_Hop_Location(const double& Wall_Pipe_Horizontal_Hop_Distance, const double& Wall_Pipe_Vertical_Hop_Distance);

	FGameplayTag Get_Wall_Pipe_Hop_Action_Based_On_Parkour_Direction(const FGameplayTag& Current_Parkour_Direction);

	void Set_Horizontal_Wall_Pipe_Hop_Distance_Value_Based_On_Parkour_Direction(const FGameplayTag& Current_Parkour_Direction);

	void Set_Vertical_Wall_Pipe_Hop_Distance_Value_Based_On_Parkour_Direction(const FGameplayTag& Current_Parkour_Direction);

	bool Validate_Foot_Contact_With_Ground(const bool& bIs_Left_Foot) const;

	bool Validate_Jumping_Destination_Ground_Surface(const bool& bCharacter_Is_Walking);

	void Detect_Balance_Traversal_Actors(const FVector& Initial_Balance_Traversal_Actor_Forward_Vector_Location, const FRotator& Initial_Balance_Traversal_Actor_Rotation, const int& Scan_Width, const int& Scan_Height);

	void Analyze_Detect_Balance_Traversal_Actors_Hit_Traces_For_Best_Hit();

	bool Validate_Balance_Traversal_Location();

	bool Parkour_Balance_Walk_Detect_Balance_Surface(FHitResult& Parkour_Balance_Walk_Detect_Balance_Surface_Hit_Result_Reference, FHitResult& Parkour_Balance_Walk_Balance_Surface_Top_Result_Reference);

	bool Parkour_Balance_Walk_Are_There_Obstacles_In_Front_Of_Feet(const FHitResult& Parkour_Balance_Walk_Balance_Surface_Top_Result_Reference);

	bool Parkour_Balance_Walk_Are_There_Obstacles_In_Front_Of_Body(const FHitResult& Parkour_Balance_Walk_Balance_Surface_Top_Result_Reference);

	bool Validate_Can_Execute_Balance_Walk_Automatic_Hop();

	void Balance_Walk_Automatic_Hop_Detect_Wall();

	bool Balance_Walk_Automatic_Hop_Calculate_Wall_Top_Surface();

	bool Validate_Balance_Walk_Automatic_Hop_Location();

	bool Validate_Balance_Drop_Hanging(const bool& bDrop_To_Left_Side);

	bool Validate_Free_Hang_To_Balanced_Walk(const bool& bClimb_To_Left_Side);

#pragma endregion

#pragma region Parkour_Core

	void Parkour_State_Settings(const ECollisionEnabled::Type& Collision_Type, const EMovementMode& New_Movement_Mode, const bool& bStop_Movement_Immediately, const bool& bIgnore_Controller_Input);

	void Set_Parkour_State_Attributes(const FGameplayTag& Current_Parkour_State);

	void Set_Parkour_State(const FGameplayTag& New_Parkour_State);

	void Set_Parkour_Climb_Style(const FGameplayTag& New_Parkour_Climb_Style);

	void Set_Parkour_Wall_Run_Side(const FGameplayTag& New_Wall_Run_Side);

	void Set_Parkour_Direction(const FGameplayTag& New_Parkour_Direction);

	void Set_Parkour_Action(const FGameplayTag& New_Parkour_Action);

	float Select_Value_Based_On_Climb_Style(const FGameplayTag& Climb_Style, const float& Braced_Value, const float& Free_Hang_Value) const;

	void Measure_Wall();

	void Decide_Parkour_Action();

	void Reset_Parkour_Variables();

	void Parkour_Call_In_Tick();

	//void Get_Parkour_Data_Asset(UParkour_Action_Data* Parkour_Action_Data);

	FVector Find_Parkour_Warp_Location(const FVector& Impact_Point_To_Use, const FRotator& Direction_For_Character_To_Face, const float& X_Axis_Offset, const float& Y_Axis_Offset, const float& Z_Axis_Offset) const;

	void Play_Parkour_Montage(UParkour_Action_Data* Parkour_Data_Asset_To_Use);

	void Function_To_Execute_On_Animation_Blending_Out(UAnimMontage *Montage, bool bInterrupted);

	void Parkour_Climb_Handle_Shimmying_Movement();

	void Calculate_And_Move_Character_To_New_Climb_Position(const FHitResult& Parkour_Climbing_Detect_Wall_Hit_Result, const FHitResult& Parkour_Climbing_Wall_Top_Result);

	void Move_Character_To_New_Climb_Position_Interpolation_Settings(const FVector& Input_Location_To_Move_Character, const FRotator& Input_Rotation_For_Character_To_Face);

	void Dynamic_IK_Limbs();

	void Reset_Parkour_IK_Hands(bool bIs_Left_Hand);

	void Reset_Parkour_IK_Feet(bool bIs_Left_Foot);

	void Handle_Release_From_Shimmying();

	void Set_bIs_Falling_To_False();

	void Set_bCan_Jump_From_Braced_Climb_To_True();

	void Set_bCan_Jump_From_Free_Hang_To_True();

	bool Validate_Can_Start_Shimmying_While_Airborne() const;

	FGameplayTag Get_Controller_Direction() const;

	void Parkour_Shimmy_Handle_Corner_Movement();

	void Parkour_Shimmy_Corner_Movement(const FHitResult& New_Corner_Detect_Wall_Hit_Result, const FHitResult& New_Corner_Wall_Top_Result);

	void Set_bOut_Corner_Movement_To_False();

	void Set_bIn_Corner_Movement_To_False();

	void Decide_Shimmy_180_Shimmy_Mantle_Or_Hop();

	void Execute_Random_Montage(TArray<UParkour_Action_Data*>& Array_To_Select_From);

	void Select_Random_Montage_To_Execute(TArray<UParkour_Action_Data*>& Array_To_Select_From);

	FGameplayTag Get_Hop_Action_Based_On_Parkour_Direction(const FGameplayTag& Current_Parkour_Direction) const;

	void Set_Horizontal_Hop_Distance_Value_Based_On_Parkour_Direction(const FGameplayTag& Current_Parkour_Direction);

	void Set_Vertical_Hop_Distance_Value_Based_On_Parkour_Direction(const FGameplayTag& Current_Parkour_Direction);

	float Select_Value_Based_On_Parkour_State(const FGameplayTag& Current_Parkour_State, const float& Parkour_State_Free_Roam_Value_To_Return, 
																	  const float& Parkour_State_Ready_To_Climb_Value_To_Return, 
																	  const float& Parkour_State_Climb_Value_To_Return) const;

	double Get_Characters_Highest_Hand_Height() const;

	bool Realize_And_Validate_Hop_Destnation_And_Action();

	void Perform_Hop_Action(const FGameplayTag& Hop_Action);

	void Reset_Wall_Run_Variables_And_Set_Parkour_State_To_Free_Roam();

	void Calculate_And_Move_Character_To_New_Wall_Run_Position(const FHitResult& Wall_Running_Hit_Result);
	
	void Move_Character_To_New_Wall_Run_Position_Interpolation_Settings(const FVector& Location_To_Move_Character, const FRotator& Rotation_For_Character_To_Face);

	void Parkour_Wall_Run_Movement();

	void Dynamic_Wall_Run_Arc_Path();

	void Set_bCan_Jump_From_Wall_Run_To_True();

	void Stabilize_Movement_While_Free_Roaming();

	void On_Landing_Impact();

	void Move_Character_To_Front_Of_Pipe();

	void Execute_Accelerating_Drop_Free_Roam();

	void Parkour_Wall_Pipe_Climb_Handle_Pipe_Climbing_Movement();

	void Calculate_And_Move_Character_To_New_Wall_Pipe_Climb_Position(const FHitResult& Parkour_Wall_Pipe_Climbing_Wall_Top_Result_Reference);

	void Decide_Wall_Pipe_Maneuver_To_Free_Hang_Mantle_Or_Hop();

	bool Validate_Wall_Pipe_Climb_Hop_Destnation_And_Action();

	void Parkour_Balance_Walk_Handle_Balance_Walking_Movement();

	void Calculate_And_Move_Character_To_New_Balance_Walk_Position(const FHitResult& Parkour_Balance_Walk_Balance_Surface_Top_Result_Reference);

	void Move_Character_To_New_Balance_Walk_Position_Interpolation_Settings(const FVector& Location_To_Move_Character, const FRotator& Rotation_For_Character_To_Face);

	#pragma region Set_Network_Variables

	void Set_Network_Wall_Calculations(const double& Network_Wall_Height, const double& Network_Wall_Depth, const double& Network_Vault_Height);

	void Set_Network_Variables(const FHitResult& Network_Wall_Top_Result, const FRotator& Network_Reversed_Front_Wall_Normal_Z, const FHitResult& Custom_Wall_Pipe_Forward_Vector);

	void Set_Network_Initialize_Parkour_IK_Limbs(const FHitResult& Network_Initialize_Parkour_IK_Limbs_Hit_Result);

	void Set_Network_Move_Character_To_Front_Of_Pipe(const FVector& Location_To_Move_Character, const FRotator& Rotation_For_Character_To_Face);

	#pragma endregion

#pragma endregion

#pragma region Parkour_Core_Variables

	#pragma region Gameplay_Tags
	
	UPROPERTY(Replicated)
	FGameplayTag Parkour_State{FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Free.Roam")))};

	UPROPERTY(Replicated)
	FGameplayTag Parkour_Climb_Style{FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Climb.Style.None")))};

	UPROPERTY(Replicated)
	FGameplayTag Parkour_Wall_Run_Side{FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Wall.Run.Side.None")))};

	UPROPERTY(Replicated)
	FGameplayTag Parkour_Direction{FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.None")))};

	UPROPERTY(Replicated)
	FGameplayTag Parkour_Action{FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.No.Action")))};

	#pragma endregion

	#pragma region Hit_Results

	FHitResult Initial_Front_Wall_Hit_Result{};
	
	TArray<FHitResult> Grid_Scan_Hit_Traces{};
	
	TArray<FHitResult> Front_Wall_Top_Edge_Traces{};

	FHitResult Front_Wall_Top_Edge_Best_Hit{};

	UPROPERTY(Replicated)
	FRotator Reversed_Front_Wall_Normal_Z{};

	UPROPERTY(Replicated)
	FHitResult Wall_Top_Result{};

	FHitResult Wall_Depth_Result{};

	FHitResult Wall_Vault_Result{};

	UPROPERTY(Replicated)
	FHitResult Initialize_Parkour_IK_Limbs_Hit_Result{};

	FHitResult Parkour_Shimmying_Detect_Out_Corner_Wall_Hit_Result{};

	FHitResult Parkour_Shimmying_Out_Corner_Wall_Top_Result{};

	FHitResult Parkour_Shimmying_Detect_In_Corner_Wall_Hit_Result{};

	FHitResult Parkour_Shimmying_In_Corner_Wall_Top_Result{};

	TArray<FHitResult> Grid_Scan_Hop_Hit_Traces{};

	TArray<FHitResult> Front_Wall_Top_Edge_Hop_Traces{};

	FHitResult Front_Wall_Top_Edge_Best_Hop_Hit{};

	FHitResult Hop_Top_Hit_Result{};

	FHitResult Realize_Wall_Run_Left_Side_Hit_Result{};

	FHitResult Realize_Wall_Run_Right_Side_Hit_Result{};

	FHitResult Wall_Run_Hit_Result{};

	FHitResult Realize_Wall_Pipe_Hit_Result{};

	UPROPERTY(Replicated)
	FHitResult Custom_Wall_Pipe_Actor_Forward_Vector_Hit_Result{};

	TArray<FHitResult> Detect_Balance_Traversal_Actors_Hit_Traces{};

	FHitResult Balance_Traversal_Actors_Best_Hit{};

	FHitResult Balance_Walk_Automatic_Hop_Top_Result{};
	
	#pragma endregion

	#pragma region Others

	UPROPERTY(Replicated)
	float Ground_Speed{};

	UPROPERTY(Replicated)
	float Air_Speed{};

	double Distance_In_Grid_Scan_For_Hit_Results_Current_Iteration{};

	double Distance_In_Grid_Scan_For_Hit_Results_Previous_Iteration{};

	UPROPERTY()
	bool bIs_On_Ground{false};

	UPROPERTY(Replicated)
	bool bIs_Falling{false};

	FTimerHandle Set_bIs_Falling_To_False_Timer_Handle{};

	const float Set_bIs_Falling_To_False_Timer_Duration{1.f};

	bool bCan_Jump_From_Braced_Climb{true};

	FTimerHandle Set_bCan_Jump_From_Braced_Climb_Timer_Handle{};

	const float Set_bCan_Jump_From_Braced_Climb_Timer_Duration{1.f};

	bool bCan_Jump_From_Free_Hang{true};

	FTimerHandle Set_bCan_Jump_From_Free_Hang_Timer_Handle{};

	const float Set_bCan_Jump_From_Free_Hang_Timer_Duration{1.f};

	FLatentActionInfo Corner_Movement_Latent_Action_Info{};

	bool bOut_Corner_Movement{false};

	FTimerHandle Set_bOut_Corner_Movement_To_False_Timer_Handle{};

	const float Set_bOut_Corner_Movement_To_False_Timer_Duration{.5f};

	bool bIn_Corner_Movement{false};

	FTimerHandle Set_bIn_Corner_Movement_To_False_Timer_Handle{};

	const float Set_bIn_Corner_Movement_To_False_Timer_Duration{.5f};

	double Distance_In_Grid_Scan_For_Hop_Hit_Results_Current_Iteration{};

	double Distance_In_Grid_Scan_For_Hop_Hit_Results_Previous_Iteration{};

	float Horizontal_Hop_Distance{};

	float Vertical_Hop_Distance{};

	UPROPERTY(Replicated)
	int Random_Montage_To_Play{};

	UPROPERTY(Replicated)
	int Last_Random_Montage_Played{};

	FVector Direction_To_Wall_Run{};

	FVector Location_To_Move_Charater_During_Wall_Run{};

	float Direction_To_Wall_Run_Scalar_Value_Multiplier{1.f};

	float Dynamic_Wall_Run_Arc_Value{};

	bool bCan_Jump_From_Wall_Run{true};

	FTimerHandle Set_bCan_Jump_From_Wall_Run_Timer_Handle{};

	float Set_bCan_Jump_From_Wall_Run_Timer_Duration{1.f};

	bool Do_Once_1{true};

	bool Do_Once_2{true};

	bool bReady_To_Initialize_Parkour_Wall_Pipe{false};

	FVector Wall_Pipe_Forward_Vector{};

	bool bAccurate_Jump_Destination_Found{false};

	bool bParkour_Action_Jump_Finish_On_Blending_Out{false};
	
	#pragma endregion

	#pragma region Measure_Wall

	UPROPERTY(Replicated)
	double Wall_Height{};

	UPROPERTY(Replicated)
	double Wall_Depth{};

	UPROPERTY(Replicated)
	double Vault_Height{};

	#pragma endregion
	
	#pragma region Data_Assets_Arrays


	TArray<UParkour_Action_Data*> Braced_And_Ledge_Shimmy_180_Shimmy_Array{/* Climb_Shimmy_To_Shimmy_180_Vault,
																		   Ledge_Turn_L_Vault,
																		   Ledge_Turn_R_Vault */};

	TArray<UParkour_Action_Data*> Hanging_Shimmy_180_Shimmy_Array{/* Hanging_180_L,
																  Hanging_180_R */};

	TArray<UParkour_Action_Data*> Ledge_Climb_Up_Array{/* Ledge_Climb_Up_Reverse, 
													   Ledge_Climb_Up_TwoHand_L,
													   Ledge_Climb_Up_TwoHand_R,
													   Ledge_Climb_Up_Monkey, 
													   Climb_Up_The_Ledge */};

	TArray<UParkour_Action_Data*> Hanging_Climb_Up_Array{/* Hanging_Climb_Up,
														 Free_Hang_Climb_Up */};

	TArray<UParkour_Action_Data*> Hop_Up_Array{/* Braced_Hang_Hop_Up,
											   Ledge_Jump_Up_Power,
											   Ledge_Jump_Up,
											   Climb_Shimmy_Long_L_Up,
											   Climb_Shimmy_Long_R_Up */};

	TArray<UParkour_Action_Data*> Braced_And_Ledge_Hop_Left_Array{/* Braced_Hang_Hop_Left,
																  Ledge_Jump_L_Short,
																  Ledge_Jump_L,
																  Climb_Shimmy_Long_L_Left */};

	TArray<UParkour_Action_Data*> Braced_And_Ledge_Hop_Right_Array{/* Braced_Hang_Hop_Right,
																   Ledge_Jump_R_Short,
																   Ledge_Jump_R,
																   Climb_Shimmy_Long_R_Right */};


	TArray<UParkour_Action_Data*> Braced_And_Adventure_Hop_Up_Left_Array{/*Braced_Hang_Hop_Left_Up,
																   Climb_Shimmy_Long_L_Up_Left */};

	TArray<UParkour_Action_Data*> Braced_And_Adventure_Hop_Up_Right_Array{/* Braced_Hang_Hop_Right_Up,
																   Climb_Shimmy_Long_R_Up_Right */};

	TArray<UParkour_Action_Data*> Braced_And_Ledge_Hop_Down_Array{/* Braced_Hang_Hop_Down,
																  Ledge_Jump_Down,
															      Climb_Leap_Down_To_Ledge */};

	TArray<UParkour_Action_Data*> Exit_Ledge_Jump_Forward_Array{/* Ledge_Climb_Up_Monkey_Vault,
												        		Ledge_Climb_Up_Reverse_L_Vault,
																Ledge_Climb_Up_Reverse_R_Vault,
																Ledge_Climb_Up_Safety_L_Vault,
																Ledge_Climb_Up_Safety_R_Vault,
																Ledge_Climb_Up_Thief_L_Vault,
																Ledge_Climb_Up_Thief_R_Vault,
																Ledge_Climb_Up_TwoHand_L_Vault,
																Ledge_Climb_Up_TwoHand_R_Vault */};
	
	TArray<UParkour_Action_Data*> Exit_Ledge_Jump_Backward_Array{/* Exit_Ledge_Jump_Backward_L,
												        		 Exit_Ledge_Jump_Backward_R */};

	TArray<UParkour_Action_Data*> Drop_Ledge_Array{/* Accelerating_Drop_Ledge_L,
													Accelerating_Drop_Ledge_R,
													Accelerating_Drop_Slide_Ledge_L,
													Accelerating_Drop_Slide_Ledge_R */};
	
	TArray<UParkour_Action_Data*> Drop_Hanging_Array{/* Accelerating_Drop_Hanging_L,
													  Accelerating_Drop_Hanging_R */};

	TArray<UParkour_Action_Data*> Landing_Down_Front_Array{/* Landing_Front_L,
														    Landing_Front_R */};

	TArray<UParkour_Action_Data*> Landing_Down_Roll_Array{/* Landing_Roll_A_L,
														   Landing_Roll_A_R,
														   Landing_Roll_B_L,
														   Landing_Roll_B_R */};

	TArray<UParkour_Action_Data*> Free_Roam_Accelerating_Drop_Array{/* Dash_Drop,
																    Jump_Drop,
																    Monkey_Drop,
																    Reverse_L_Drop,
																    Reverse_R_Drop,
																    Speed_L_Drop,
																    Speed_R_Drop,
																    Two_Hand_L_Drop,
																    Two_Hand_R_Drop */};

	TArray<UParkour_Action_Data*> Balance_Walk_Automatic_Hop_Array{/* Balance_Walk_Jump_Front,
																	Jump_One_L,
																	Jump_One_R */};
																   
	#pragma endregion

#pragma endregion

#pragma region Parkour_BP_Variables

	#pragma region Trace_Types

	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	// int Grid_Scan_Width{4};

	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	// int Grid_Scan_Height{30};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Parkour_Detect_Wall_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Parkour_Grid_Scan_For_Hit_Results_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Parkour_Analyzing_Top_Wall_Surface_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Parkour_Wall_Depth_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Parkour_Vault_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Parkour_Validate_On_Land_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Parkour_Decide_Climb_Style_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Parkour_Climbing_Detect_Wall_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Parkour_Climbing_Wall_Top_Result_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Parkour_Climbing_Check_Sides_Of_Hands_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Parkour_Climbing_Check_Sides_Of_Body_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Parkour_Shimmying_Initialize_IK_Hands_Detect_Wall_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Parkour_Shimmying_Initialize_IK_Hands_Wall_Top_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Parkour_Shimmying_Dynamic_IK_Hands_Detect_Wall_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Parkour_Shimmying_Dynamic_IK_Hands_Wall_Top_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Parkour_Shimmying_Initialize_IK_Feet_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Parkour_Shimmying_Dynamic_IK_Feet_Detect_Wall_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Parkour_Shimmying_Validate_Out_Corner_Wall_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Parkour_Shimmying_Detect_Out_Corner_Wall_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Parkour_Shimmying_Out_Corner_Wall_Top_Result_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Parkour_Shimmying_Detect_In_Corner_Wall_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Parkour_Shimmying_In_Corner_Wall_Top_Result_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Parkour_Shimmying_Validate_In_Corner_Wall_Space_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Validate_Shimmy_180_Rotation_To_Shimmy_Detect_Wall_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Validate_Shimmy_180_Rotation_To_Shimmy_Wall_Top_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Validate_Shimmy_180_Rotation_To_Shimmy_Space_Check_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Validate_Climb_Or_Hop_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Parkour_Hop_Top_Result_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Validate_Can_Fly_Hanging_Jump_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Validate_Can_Jump_From_Braced_Climb_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Validate_Can_Jump_From_Free_Hang_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Validate_Accelerating_Drop_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Validate_Accelerating_Drop_Detect_Wall_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Validate_Accelerating_Drop_Wall_Top_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Validate_Accelerating_Drop_Space_Check_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Realize_Wall_Run_Surfaces_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Validate_Can_Jump_From_Wall_Run_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Validate_Drop_Off_Ledge_While_Sprinting_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Parkour_Detect_Wall_Pipe_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Validate_Foot_Contact_With_Ground_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Validate_Jumping_Destination_Ground_Surface_Trace_Types{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Balance_Traversal_Actors_Trace_Types{};
	

	#pragma endregion

	#pragma region Parkour_Data_Assets

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Default_Parkour_Data_Asset_Pointer{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Braced_Jump_To_Climb{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Free_Hang_Jump_To_Climb{};



	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Fall_Down{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Fall_Down_180_R{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Fall_Down_180_L{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Hanging_Drop{};



	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Braced_Jump_To_Climb_Airborne{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Leap_Entry_To_Climb_Hang_Idle{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Fly_Hanging_Jump{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Free_Hang_Jump_To_Climb_Airborne{};



	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Corner_Outer_L{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Corner_Outer_R{};


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Hanging_Corner_Outer_L{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Hanging_Corner_Outer_R{};



	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Corner_Inner_L{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Corner_Inner_R{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Hanging_Corner_Inner_L{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Hanging_Corner_Inner_R{};



	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	// TArray<UParkour_Action_Data*> Ledge_Climb_Up_Array{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_Reverse{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_TwoHand_L{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_TwoHand_R{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_Monkey{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Climb_Up_The_Ledge{};



	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	// TArray<UParkour_Action_Data*> Hanging_Climb_Up_Array{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Hanging_Climb_Up{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Free_Hang_Climb_Up{};

	

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Braced_Hang_Hop_Up{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Braced_Hang_Hop_Left{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Braced_Hang_Hop_Right{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Braced_Hang_Hop_Left_Up{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Braced_Hang_Hop_Right_Up{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Braced_Hang_Hop_Down{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Jump_Up_Power{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Jump_Up{};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Jump_L_Short{};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Jump_L{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Jump_R_Short{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Jump_R{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Jump_Down{};

	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Free_Hang_Hop_Left{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Free_Hang_Hop_Right{};



	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Climb_Shimmy_Long_L_Up{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Climb_Shimmy_Long_L_Up_Left{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Climb_Shimmy_Long_L_Down_Left{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Climb_Shimmy_Long_L_Left{};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Climb_Shimmy_Long_R_Up{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Climb_Shimmy_Long_R_Up_Right{};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Climb_Shimmy_Long_R_Down_Right{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Climb_Shimmy_Long_R_Right{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Climb_Leap_Down_To_Ledge{};




	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Climb_Shimmy_To_Shimmy_180_Vault{};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Turn_L_Vault{};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Turn_R_Vault{};



	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Hanging_180_L{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Hanging_180_R{};



	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_Monkey_Vault{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_Reverse_L_Vault{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_Reverse_R_Vault{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_Safety_L_Vault{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_Safety_R_Vault{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_Thief_L_Vault{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_Thief_R_Vault{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_TwoHand_L_Vault{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_TwoHand_R_Vault{};
	

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Exit_Ledge_Jump_Backward_L{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Exit_Ledge_Jump_Backward_R{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Exit_Hanging_Jump{};




	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Accelerating_Drop_Ledge_L{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Accelerating_Drop_Ledge_R{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Accelerating_Drop_Slide_Ledge_L{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Accelerating_Drop_Slide_Ledge_R{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Accelerating_Drop_Hanging_L{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Accelerating_Drop_Hanging_R{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Braced_Drop_Down{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* FreeHang_Drop_Down{};



	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Wall_Run_L_Start{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Wall_Run_R_Start{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Wall_Run_L_Jump_90_R{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Wall_Run_L_Jump_F{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Wall_Run_R_Jump_90_L{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Wall_Run_R_Jump_F{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Wall_Run_L_Finish{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Wall_Run_R_Finish{};



	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Landing_Down_Light{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Landing_Down_Impact{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Landing_Front_L{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Landing_Front_R{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Landing_Roll_A_L{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Landing_Roll_A_R{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Landing_Roll_B_L{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Landing_Roll_B_R{};



	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Dash_Drop{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Jump_Drop{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Monkey_Drop{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Reverse_L_Drop{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Reverse_R_Drop{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Speed_L_Drop{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Speed_R_Drop{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Two_Hand_L_Drop{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Two_Hand_R_Drop{};

	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Idle_To_Wall_Pipe_Attach{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Jumping_To_Wall_Pipe_Attach{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Wall_Pipe_Fall_Down{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Wall_Pipe_Climb_Up_2_Hand{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Wall_Pipe_Jump_Up{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Wall_Pipe_Jump_Down{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Wall_Pipe_Jump_Left{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Wall_Pipe_Jump_Right{};



	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Jump_Up{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Accurate_Jump_Start_L{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Accurate_Jump_Start_R{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Accurate_Jump_Start_L_Warp{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Accurate_Jump_Start_R_Warp{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Accurate_Jump_Finish{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Jump_Front_L_Start{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Jump_Front_R_Start{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Jump_Front_L_Start_Warp{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Jump_Front_R_Start_Warp{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Jump_Finish{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Jump_One_L{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Jump_One_R{};

	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Balance_Walk_90_L{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Balance_Walk_90_R{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Balance_Walk_180{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Balance_Walk_Jump_Front{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Balance_Drop_L_Hanging{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Balance_Drop_R_Hanging{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Hanging_Climb_Up_To_Balanced_Walk_L{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Hanging_Climb_Up_To_Balanced_Walk_R{};


	#pragma endregion

#pragma endregion

#pragma region Network

#pragma region Network_Core

virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

UFUNCTION(Server, Reliable)
void Server_Execute_Start_Running();

UFUNCTION(NetMulticast, Reliable)
void Multicast_Execute_Start_Running();

UFUNCTION(Server, Reliable)
void Server_Execute_Stop_Running();

UFUNCTION(NetMulticast, Reliable)
void Multicast_Execute_Stop_Running();

UFUNCTION(Server, Reliable)
void Server_Set_Parkour_State_Attributes(const FGameplayTag& Current_Parkour_State);

UFUNCTION(NetMulticast, Reliable)
void Multicast_Set_Parkour_State_Attributes(const FGameplayTag& Current_Parkour_State);

UFUNCTION(Server, Reliable)
void Server_Set_Parkour_State(const FGameplayTag& New_Parkour_State);

UFUNCTION(NetMulticast, Reliable)
void Multicast_Set_Parkour_State(const FGameplayTag& New_Parkour_State);

UFUNCTION(Server, Reliable)
void Server_Set_Parkour_Climb_Style(const FGameplayTag& New_Parkour_Climb_Style);

UFUNCTION(NetMulticast, Reliable)
void Multicast_Set_Parkour_Climb_Style(const FGameplayTag& New_Parkour_Climb_Style);

UFUNCTION(Server, Reliable)
void Server_Set_Parkour_Wall_Run_Side(const FGameplayTag& New_Wall_Run_Side);

UFUNCTION(NetMulticast, Reliable)
void Multicast_Set_Parkour_Wall_Run_Side(const FGameplayTag& New_Wall_Run_Side);

UFUNCTION(Server, Reliable)
void Server_Set_Parkour_Direction(const FGameplayTag& New_Parkour_Direction);

UFUNCTION(NetMulticast, Reliable)
void Multicast_Set_Parkour_Direction(const FGameplayTag& New_Parkour_Direction);

UFUNCTION(Server, Reliable)
void Server_Handle_Release_From_Shimmying(const FGameplayTag& Network_Parkour_Climb_Style, const FGameplayTag& Network_Parkour_Direction, const double& Network_Forward_Backward_Movement_Value_Absolute_Value);

UFUNCTION(NetMulticast, Reliable)
void Multicast_Handle_Release_From_Shimmying(const FGameplayTag& Network_Parkour_Climb_Style, const FGameplayTag& Network_Parkour_Direction, const double& Network_Forward_Backward_Movement_Value_Absolute_Value);

UFUNCTION(Server, Reliable)
void Server_Set_Parkour_Action(const FGameplayTag& New_Parkour_Action, const int& Network_Random_Montage_To_Play);

UFUNCTION(NetMulticast, Reliable)
void Multicast_Set_Parkour_Action(const FGameplayTag& New_Parkour_Action, const int& Network_Random_Montage_To_Play);

UFUNCTION(Server, Reliable)
void Server_Perform_Hop_Action(const FGameplayTag& Network_Parkour_State, const FGameplayTag& Network_Hop_Action, const int& Network_Random_Montage_To_Play);

UFUNCTION(NetMulticast, Reliable)
void Multicast_Perform_Hop_Action(const FGameplayTag& Network_Parkour_State, const FGameplayTag& Network_Hop_Action, const int& Network_Random_Montage_To_Play);

UFUNCTION(Server, Reliable)
void Server_Decide_Parkour_Action();

UFUNCTION(NetMulticast, Reliable)
void Multicast_Decide_Parkour_Action();

UFUNCTION(Server, Reliable)
void Server_Move_Character_To_New_Climb_Position_Interpolation_Settings(const FVector& Location_To_Move_Character, const FRotator& Rotation_For_Character_To_Face);

UFUNCTION(NetMulticast, Reliable)
void Multicast_Move_Character_To_New_Climb_Position_Interpolation_Settings(const FVector& Location_To_Move_Character, const FRotator& Rotation_For_Character_To_Face);

UFUNCTION(Server, Reliable)
void Server_Move_Character_To_Front_Of_Pipe(const FVector& Location_To_Move_Character, const FRotator& Rotation_For_Character_To_Face);

UFUNCTION(NetMulticast, Reliable)
void Multicast_Move_Character_To_Front_Of_Pipe(const FVector& Location_To_Move_Character, const FRotator& Rotation_For_Character_To_Face);

UFUNCTION(Server, Reliable)
void Server_Move_Character_To_New_Balance_Walk_Position_Interpolation_Settings(const FVector& Location_To_Move_Character, const FRotator& Rotation_For_Character_To_Face);

UFUNCTION(NetMulticast, Reliable)
void Multicast_Move_Character_To_New_Balance_Walk_Position_Interpolation_Settings(const FVector& Location_To_Move_Character, const FRotator& Rotation_For_Character_To_Face);

#pragma endregion

#pragma region Set_Network_Variables

UFUNCTION(Server, Reliable)
void Server_Set_Network_Wall_Calculations(const double& Network_Wall_Height, const double& Network_Wall_Depth, const double& Network_Vault_Height);

UFUNCTION(NetMulticast, Reliable)
void Multicast_Set_Network_Wall_Calculations(const double& Network_Wall_Height, const double& Network_Wall_Depth, const double& Network_Vault_Height);

UFUNCTION(Server, Reliable)
void Server_Set_Network_Variables(const FHitResult& Network_Wall_Top_Result, const FRotator& Network_Reversed_Front_Wall_Normal_Z, const FHitResult& Custom_Wall_Pipe_Forward_Vector);

UFUNCTION(NetMulticast, Reliable)
void Multicast_Set_Network_Variables(const FHitResult& Network_Wall_Top_Result, const FRotator& Network_Reversed_Front_Wall_Normal_Z, const FHitResult& Custom_Wall_Pipe_Forward_Vector);

UFUNCTION(Server, Reliable)
void Server_Set_bIs_Falling_To_True();

UFUNCTION(Server, Reliable)
void Server_Set_bIs_Falling_To_False();


#pragma endregion

#pragma endregion

#pragma endregion



public:

#pragma region Climbing

	void Toggle_Climbing(bool B_Eneble_Climb);
	void Request_Hopping();
	bool Is_Climbing() const;
	FORCEINLINE FVector Get_Climbable_Surface_Normal() const {return Current_Climbable_Surface_Normal;}
	FVector Get_Unrotated_Climb_Velocity() const;
 #pragma endregion

#pragma region Take Cover

	void Toggle_Take_Cover(bool bEneble_Take_Cover);
	bool Is_Taking_Cover() const;
	FORCEINLINE FVector Get_Take_Cover_Surface_Normal() const {return Current_Take_Cover_Surface_Normal;}
	FVector Get_Unrotated_Take_Cover_Velocity() const;

#pragma endregion

#pragma region Initializing_Starting_And_Ending_Parkour

	void Initialize_Parkour(ATechnical_Animator_Character* Character, UMotionWarpingComponent* Motion_Warping, UCameraComponent* Camera);

	void Add_Movement_Input(const FVector2D& Scale_Value, const bool& bIs_Forward_Backward_Movement);
	
	void Stop_Parkour_Movement_Immediately_And_Reset_Movement_Input_Variables();

	FORCEINLINE void Set_Parkour_Climb_Initialize_IK_Hands(const bool& bIs_Left_Hand) {Parkour_Climb_Initialize_IK_Hands(bIs_Left_Hand);}

	FORCEINLINE void Set_Parkour_Climb_Initialize_IK_Feet(const bool& bIs_Left_Foot) {Parkour_Climb_Initialize_IK_Feet(bIs_Left_Foot);}
	
	void Execute_Parkour_Action();

	void Release_From_Shimmying();

	void Execute_Jump_Out_Of_Shimmy();

	void Execute_Drop_Into_Shimmy();

	void Execute_Wall_Run();

	void Execute_Exit_Wall_Run_With_Jump_Forward();

	void Execute_Parkour_Wall_Pipe_Climb();

	void Execute_Parkour_Wall_Pipe_Climb_Action();

	void Execute_Start_Running();

	void Execute_Stop_Running();

	void Execute_Parkour_Jump();

	void Execute_Balance_Traversal(ABalance_Traversal_Actor* Balance_Traversal_Actor_Reference);

	void Execute_Exit_Balance_Traversal();

	void Execute_Balance_Drop_Hanging();

	void Execute_Free_Hang_To_Balanced_Walk();

	FORCEINLINE void Set_Parkour_Wall_Pipe_Climb_Initialize_IK_Hands(const bool& bIs_Left_Hand) {Parkour_Wall_Pipe_Climb_Initialize_IK_Hands(bIs_Left_Hand);}

	FORCEINLINE void Set_Parkour_Wall_Pipe_Climb_Initialize_IK_Feet(const bool& bIs_Left_Foot) {Parkour_Wall_Pipe_Climb_Initialize_IK_Feet(bIs_Left_Foot);}

	void Release_From_Parkour_Wall_Pipe_Climb();

	FORCEINLINE bool Get_bIs_Falling() const {return bIs_Falling;}

	UPROPERTY(Replicated)
	double Forward_Backward_Movement_Value{};

	UPROPERTY(Replicated)
	double Right_Left_Movement_Value{};

	FVector Current_Input_Vector{};

	FRotator Current_Input_Rotation{};

	FRotator Target_Input_Rotation{};

	double Interpolated_Forward_Backward_Movement_Value{};

	double Interpolated_Right_Left_Movement_Value{};

	FORCEINLINE FGameplayTag Get_Parkour_State() const {return Parkour_State;}

	EDrawDebugTrace::Type Debug_Action{EDrawDebugTrace::None};


#pragma endregion

};