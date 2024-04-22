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
class APlayerController;
class UParkour_Action_Data;

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
	UAnimInstance* Owning_Player_Animation_Instance;

	UPROPERTY()
	ATechnical_Animator_Character* Owning_Player_Character;

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
	ACharacter_Direction_Arrow* Character_Direction_Arrow;

	UPROPERTY()
	UCharacterMovementComponent* Character_Movement;

	UPROPERTY()
	USkeletalMeshComponent* Mesh;

	UPROPERTY()
	UCapsuleComponent* Capsule_Component;

	UPROPERTY()
	UAnimInstance* Anim_Instance;

	UPROPERTY()
	UMotionWarpingComponent* Motion_Warping_Component;

	UPROPERTY()
	UCameraComponent* Camera_Component;

	UPROPERTY()
	APlayerController* Player_Controller;
	
	IParkour_Locomotion_Interface* Parkour_Interface;

	UPROPERTY()
	UParkour_Action_Data* Parkour_Data_Asset;

#pragma endregion

#pragma region Parkour_Traces

	void Parkour_Detect_Wall();

	void Grid_Scan_For_Hit_Results(const FVector& Base_Location, const FRotator& Previous_Trace_Rotation, const int& Scan_Width_Value, const int& Scan_Height_Value);

	void Analyze_Grid_Scan_For_Front_Wall_Top_Edge_Hits();

	void Realize_Front_Wall_Top_Edge_Best_Hit();

	void Analyze_Wall_Top_Surface();

	void Calculate_Wall_Top_Surface();

	void Calculate_Wall_Vault_Location();

	void Calculate_Wall_Height();
	
	void Calculate_Wall_Depth();
	
	void Calculate_Vault_Height();

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

	void Get_Hop_Top_Result();

	bool Validate_Can_Fly_Hanging_Jump() const;

	bool Validate_Can_Jump_From_Braced_Climb(const bool& bJump_Forward) const;

	bool Validate_Can_Jump_From_Free_Hang() const;

	bool Validate_Drop_To_Shimmy(const int& Maximum_Distance_To_Check_For_Drop);

	void Realize_Wall_Run_Surfaces(const bool& bAnalyze_Characters_Left_Side);

	void Assign_Wall_Run_Hit_Result(const FHitResult& Wall_Found_On_Left_Side, const FHitResult& Wall_Found_On_Right_Side);

	bool Analyze_And_Validate_Wall_Run_Surface();

	void Wall_Run_Detect_Wall(const bool& bAnalyze_Characters_Left_Side);
	
	bool Validate_Can_Jump_From_Wall_Run() const;

#pragma endregion

#pragma region Parkour_Core

	void Parkour_State_Settings(const ECollisionEnabled::Type& Collision_Type, const EMovementMode& New_Movement_Mode, const bool& bStop_Movement_Immediately, const bool& bIgnore_Controller_Input);

	void Set_Parkour_State_Attributes(const FGameplayTag& Current_Parkour_State);

	void Set_Parkour_State(const FGameplayTag& New_Parkour_State);

	void Set_Parkour_Climb_Style(const FGameplayTag& New_Climb_Style);

	void Set_Parkour_Wall_Run_Side(const FGameplayTag& New_Wall_Run_Side);

	void Set_Parkour_Direction(const FGameplayTag& New_Direction);

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

	void Reset_Parkour_IK_Feet(bool bIs_Left_Hand);

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


#pragma endregion

#pragma region Parkour_Core_Variables

	#pragma region Gameplay_Tags
	
	FGameplayTag Parkour_State{FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Free.Roam")))};

	FGameplayTag Parkour_Climb_Style{FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Climb.Style.None")))};

	FGameplayTag Parkour_Wall_Run_Side{FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Wall.Run.Side.None")))};

	FGameplayTag Parkour_Direction{FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.None")))};

	FGameplayTag Parkour_Action{FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.No.Action")))};

	#pragma endregion

	#pragma region Hit_Results

	FHitResult Initial_Front_Wall_Hit_Result{};
	
	TArray<FHitResult> Grid_Scan_Hit_Traces{};
	
	TArray<FHitResult> Front_Wall_Top_Edge_Traces{};

	FHitResult Front_Wall_Top_Edge_Best_Hit{};

	FRotator Reversed_Front_Wall_Normal_Z{};

	FHitResult Wall_Top_Result{};

	FHitResult Wall_Depth_Result{};

	FHitResult Wall_Vault_Result{};

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
	
	#pragma endregion

	#pragma region Others

	float Ground_Speed{};

	float Air_Speed{};
	
	double Distance_In_Grid_Scan_For_Hit_Results_Current_Iteration{};

	double Distance_In_Grid_Scan_For_Hit_Results_Previous_Iteration{};

	bool bIs_On_Ground{false};

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
	
	#pragma endregion

	#pragma region Measure_Wall

	double Wall_Height{};

	double Wall_Depth{};

	double Vault_Height{};

	#pragma endregion
	
	#pragma region Data_Assets_TArrays


	TArray<UParkour_Action_Data*> Braced_And_Ledge_Shimmy_180_Shimmy_Array{Climb_Shimmy_To_Shimmy_180_Vault,
																		   Ledge_Turn_L_Vault,
																		   Ledge_Turn_R_Vault};

	TArray<UParkour_Action_Data*> Hanging_Shimmy_180_Shimmy_Array{Hanging_180_L,
																  Hanging_180_R};

	TArray<UParkour_Action_Data*> Ledge_Climb_Up_Array{Ledge_Climb_Up_Reverse, 
													   Ledge_Climb_Up_TwoHand_L,
													   Ledge_Climb_Up_TwoHand_R,
													   Ledge_Climb_Up_Monkey, 
													   Climb_Up_The_Ledge};

	TArray<UParkour_Action_Data*> Hanging_Climb_Up_Array{Hanging_Climb_Up,
														 Free_Hang_Climb_Up};

	TArray<UParkour_Action_Data*> Hop_Up_Array{Braced_Hang_Hop_Up,
											   Ledge_Jump_Up_Power,
											   Ledge_Jump_Up,
											   Climb_Shimmy_Long_L_Up,
											   Climb_Shimmy_Long_R_Up};

	TArray<UParkour_Action_Data*> Braced_And_Ledge_Hop_Left_Array{Braced_Hang_Hop_Left,
																  Ledge_Jump_L_Short,
																  Ledge_Jump_L,
																  Climb_Shimmy_Long_L_Left};

	TArray<UParkour_Action_Data*> Braced_And_Ledge_Hop_Right_Array{Braced_Hang_Hop_Right,
																   Ledge_Jump_R_Short,
																   Ledge_Jump_R,
																   Climb_Shimmy_Long_R_Right};

	TArray<UParkour_Action_Data*> Braced_And_Ledge_Hop_Down_Array{Braced_Hang_Hop_Down,
															Ledge_Jump_Down,
															Climb_Leap_Down_To_Ledge};

	TArray<UParkour_Action_Data*> Braced_And_Adventure_Hop_Up_Left_Array{/*Braced_Hang_Hop_Left_Up,*/
																   Climb_Shimmy_Long_L_Up_Left};

	TArray<UParkour_Action_Data*> Braced_And_Adventure_Hop_Up_Right_Array{/*Braced_Hang_Hop_Right_Up,*/
																   Climb_Shimmy_Long_R_Up_Right};

	TArray<UParkour_Action_Data*> Exit_Ledge_Jump_Forward_Array{Ledge_Climb_Up_Monkey_Vault,
												        		Ledge_Climb_Up_Reverse_L_Vault,
																Ledge_Climb_Up_Reverse_R_Vault,
																Ledge_Climb_Up_Safety_L_Vault,
																Ledge_Climb_Up_Safety_R_Vault,
																Ledge_Climb_Up_Thief_L_Vault,
																Ledge_Climb_Up_Thief_R_Vault,
																Ledge_Climb_Up_TwoHand_L_Vault,
																Ledge_Climb_Up_TwoHand_R_Vault};
	
	TArray<UParkour_Action_Data*> Exit_Ledge_Jump_Backward_Array{Exit_Ledge_Jump_Backward_L,
												        Exit_Ledge_Jump_Backward_R};

	TArray <UParkour_Action_Data*> Drop_Ledge_Array{Accelerating_Drop_Ledge_L,
													Accelerating_Drop_Ledge_R,
													Accelerating_Drop_Slide_Ledge_L,
													Accelerating_Drop_Slide_Ledge_R};
	
	TArray <UParkour_Action_Data*> Drop_Hanging_Array{Accelerating_Drop_Hanging_L,
													  Accelerating_Drop_Hanging_R};

	TArray <UParkour_Action_Data*> Landing_Down_Front_Array{Landing_Front_L,
														    Landing_Front_R};

	TArray <UParkour_Action_Data*> Landing_Down_Roll_Array{Landing_Roll_A_L,
														   Landing_Roll_A_R,
														   Landing_Roll_B_L,
														   Landing_Roll_B_R};
																   
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

	#pragma endregion


	#pragma region Parkour_Data_Assets

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Default_Parkour_Data_Asset_Pointer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Braced_Jump_To_Climb;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Free_Hang_Jump_To_Climb;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Fall_Down;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Fall_Down_180_R;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Fall_Down_180_L;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Hanging_Drop;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Braced_Jump_To_Climb_Airborne;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Leap_Entry_To_Climb_Hang_Idle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Fly_Hanging_Jump;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Free_Hang_Jump_To_Climb_Airborne;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Corner_Outer_L;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Corner_Outer_R;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Hanging_Corner_Outer_L;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Hanging_Corner_Outer_R;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Corner_Inner_L;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Corner_Inner_R;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Hanging_Corner_Inner_L;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Hanging_Corner_Inner_R;


	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	// TArray<UParkour_Action_Data*> Ledge_Climb_Up_Array{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_Reverse;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_TwoHand_L;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_TwoHand_R;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_Monkey;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Climb_Up_The_Ledge;


	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	// TArray<UParkour_Action_Data*> Hanging_Climb_Up_Array{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Hanging_Climb_Up;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Free_Hang_Climb_Up;

	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Braced_Hang_Hop_Up;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Braced_Hang_Hop_Left;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Braced_Hang_Hop_Right;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Braced_Hang_Hop_Left_Up;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Braced_Hang_Hop_Right_Up;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Braced_Hang_Hop_Down;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Jump_Up_Power;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Jump_Up;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Jump_L_Short;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Jump_L;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Jump_R_Short;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Jump_R;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Jump_Down;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Free_Hang_Hop_Left;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Free_Hang_Hop_Right;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Climb_Shimmy_Long_L_Up;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Climb_Shimmy_Long_L_Up_Left;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Climb_Shimmy_Long_L_Down_Left;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Climb_Shimmy_Long_L_Left;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Climb_Shimmy_Long_R_Up;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Climb_Shimmy_Long_R_Up_Right;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Climb_Shimmy_Long_R_Down_Right;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Climb_Shimmy_Long_R_Right;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Climb_Leap_Down_To_Ledge;



	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Climb_Shimmy_To_Shimmy_180_Vault;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Turn_L_Vault;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Turn_R_Vault;



	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Hanging_180_L;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Hanging_180_R;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_Monkey_Vault;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_Reverse_L_Vault;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_Reverse_R_Vault;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_Safety_L_Vault;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_Safety_R_Vault;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_Thief_L_Vault;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_Thief_R_Vault;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_TwoHand_L_Vault;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Ledge_Climb_Up_TwoHand_R_Vault;
	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Exit_Ledge_Jump_Backward_L;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Exit_Ledge_Jump_Backward_R;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Exit_Hanging_Jump;



	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Accelerating_Drop_Ledge_L;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Accelerating_Drop_Ledge_R;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Accelerating_Drop_Slide_Ledge_L;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Accelerating_Drop_Slide_Ledge_R;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Accelerating_Drop_Hanging_L;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Accelerating_Drop_Hanging_R;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Braced_Drop_Down;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* FreeHang_Drop_Down;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Wall_Run_L_Start;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Wall_Run_R_Start;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Wall_Run_L_Jump_90_R;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Wall_Run_L_Jump_F;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Wall_Run_R_Jump_90_L;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Wall_Run_R_Jump_F;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Wall_Run_L_Finish;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Wall_Run_R_Finish;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Landing_Down_Light;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Landing_Down_Impact;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Landing_Front_L;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Landing_Front_R;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Landing_Roll_A_L;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Landing_Roll_A_R;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Landing_Roll_B_L;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
	UParkour_Action_Data* Landing_Roll_B_R;


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

	void Attach_Arrow_Actor_To_Character(ATechnical_Animator_Character* Character);

	void Get_Pointer_To_Parkour_Locomotion_Interface_Class();

	//void Get_Pointer_To_Parkour_Action_Data_Class();
	
	void Initialize_Parkour_Pointers(ATechnical_Animator_Character* Character, UMotionWarpingComponent* Motion_Warping, UCameraComponent* Camera);

	void Add_Movement_Input(const FVector2D& Scale_Value, const bool& bIs_Forward_Backward_Movement);
	
	void Stop_Parkour_Climb_Movement_Immediately_And_Reset_Movement_Input_Variables();

	FORCEINLINE void Set_Parkour_Climb_Initialize_IK_Hands(const bool& bIs_Left_Hand) {Parkour_Climb_Initialize_IK_Hands(bIs_Left_Hand);}

	FORCEINLINE void Set_Parkour_Climb_Initialize_IK_Feet(const bool& bIs_Left_Foot) {Parkour_Climb_Initialize_IK_Feet(bIs_Left_Foot);}
	
	void Execute_Parkour_Action();

	void Release_From_Shimmying();

	void Execute_Jump_Out_Of_Shimmy();

	void Execute_Drop_Into_Shimmy();

	void Execute_Wall_Run();

	void Execute_Exit_Wall_Run_With_Jump_Forward();

	FORCEINLINE bool Get_bIs_Falling() const {return bIs_Falling;}

	double Forward_Backward_Movement_Value{};

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