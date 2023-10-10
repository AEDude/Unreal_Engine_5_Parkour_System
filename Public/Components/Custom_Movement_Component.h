// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Custom_Movement_Component.generated.h"

DECLARE_DELEGATE(F_On_Enter_Climb_State)
DECLARE_DELEGATE(F_On_Exit_Climb_State)

class UAnimMontage;
class UAnimInstance;
class ATechnical_AnimatorCharacter;

UENUM(BlueprintType)
namespace E_Custom_Movement_Mode
{
	enum Type
	{
		MOVE_Climb UMETA(DisplayName = "Climb Mode")
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
protected:

#pragma region Overriden_Functions
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations);
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxAcceleration() const override;
	virtual FVector ConstrainAnimRootMotionVelocity(const FVector& RootMotionVelocity, const FVector& CurrentVelocity) const override;

#pragma endregion

private:

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

UPROPERTY()
UAnimInstance* Owning_Player_Animation_Instance;

UPROPERTY()
ATechnical_AnimatorCharacter* Owning_Player_Character;

#pragma endregion

#pragma region ClimbBPVariables
	
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

public:
	void Toggle_Climbing(bool B_Eneble_Climb);
	void Request_Hopping();
	bool Is_Climbing() const;
	FORCEINLINE FVector Get_Climbable_Surface_Normal() const {return Current_Climbable_Surface_Normal;}
	FVector Get_Unrotated_Climb_Velocity() const;

};
