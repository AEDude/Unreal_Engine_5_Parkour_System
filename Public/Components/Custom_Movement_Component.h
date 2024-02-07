// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gameplay_Tags/Gameplay_Tags.h"
#include "Custom_Movement_Component.generated.h"

DECLARE_DELEGATE(F_On_Enter_Climb_State)
DECLARE_DELEGATE(F_On_Exit_Climb_State)
DECLARE_DELEGATE(F_On_Enter_Take_Cover_State)
DECLARE_DELEGATE(F_On_Exit_Take_Cover_State)

class UCharacterMovementComponent;
class UCapsuleComponent;
class ATechnical_AnimatorCharacter;
class USkeletalMeshComponent;
class UMotionWarpingComponent;
class UAnimInstance;
class UAnimMontage;
class UCameraComponent;
class ACharacter_Direction_Arrow;
class IParkour_Locomotion_Interface;

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
	ATechnical_AnimatorCharacter* Owning_Player_Character;

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

	FVector Move_Vector_Up(const FVector& Initial_Location, const FRotator& Rotation, const float& Move_Value);

	FVector Move_Vector_Down(const FVector& Initial_Location, const FRotator& Rotation, const float& Move_Value);

	FVector Move_Vector_Left(const FVector& Initial_Location, const FRotator& Rotation, const float& Move_Value);

	FVector Move_Vector_Right(const FVector& Initial_Location, const FRotator& Rotation, const float& Move_Value);

	FVector Move_Vector_Forward(const FVector& Initial_Location, const FRotator& Rotation, const float& Move_Value);

	FVector Move_Vector_Backward(const FVector& Initial_Location, const FRotator& Rotation, const float& Move_Value);

	FRotator Add_Rotator(const FRotator& Initial_Rotation, const float& Add_To_Rotation);

	FRotator Reverse_Wall_Normal_Rotation_Z(const FVector& Initial_Wall_Normal);

	void Draw_Debug_Sphere(const FVector& Location, const float& Radius, const FColor& Color, const float& Duration, const bool& bDraw_Debug_Shape_Persistent, const float& Lifetime);

#pragma endregion

#pragma region Parkour_Pointers

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
	
	IParkour_Locomotion_Interface* Parkour_Interface;

#pragma endregion

#pragma region Parkour_Traces

	FHitResult Parkour_Detect_Wall();

#pragma endregion

#pragma region Parkour_Core

	void Parkour_State_Settings(const ECollisionEnabled::Type& Collision_Type, const EMovementMode& New_Movement_Mode, const bool& bStop_Movement_Immediately);

	void Set_Parkour_State(const FGameplayTag& New_Parkour_State);

	void Setting_Parkour_State(const FGameplayTag& Current_Parkour_State);

	void Set_Parkour_Climb_Style(const FGameplayTag& New_Climb_Style);

	void Set_Parkour_Direction(const FGameplayTag& New_Climb_Direction);

	float Climb_Style_Values(const FGameplayTag& Climb_Style, const float& Braced_Value, const float& Free_Hang_Value) const;

#pragma endregion

#pragma region Parkour_Core_Variables

	FGameplayTag Parkour_State{FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Free.Roam")))};

	FGameplayTag Parkour_Climb_Style{FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Climb.Style.Braced.Climb")))};

	FGameplayTag Parkour_Climb_Direction{FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.None")))};

	FGameplayTag Parkour_Action{FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.No.Action")))};

#pragma endregion

#pragma region Parkour_BP_Variables

UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true"))
TArray<TEnumAsByte<EObjectTypeQuery> > Parkour_Detect_Wall_Trace_Types;

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

#pragma region Initializing_And_Starting_Parkour

	void Initialize_Parkour_Pointers(ATechnical_AnimatorCharacter* Character, UMotionWarpingComponent* Motion_Warping, UCameraComponent* Camera);

	void Start_Parkour_Action();

#pragma endregion

};