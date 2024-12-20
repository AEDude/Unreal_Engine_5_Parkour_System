// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/Parkour_Locomotion_Interface.h"
#include "Logging/LogMacros.h"
#include "Technical_Animator_Character.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UCustom_Movement_Component;
class UMotionWarpingComponent;
class UCharacter_Animation_Instance;
class ACharacter_Direction_Arrow;
class AWall_Pipe_Actor;
class ABalance_Traversal_Actor;
class AWall_Vault_Actor;
class ATic_Tac_Actor;
class AStairs_Actor;

struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ATechnical_Animator_Character : public ACharacter

{
	GENERATED_BODY()

public:
	ATechnical_Animator_Character(const FObjectInitializer& ObjectInitializer);

private:

#pragma region Components

	UPROPERTY()
	ATechnical_Animator_Character* Character_Reference{};

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* Camera_Boom{};

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Follow_Camera{};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UCustom_Movement_Component* Custom_Movement_Component{};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UMotionWarpingComponent* Motion_Warping_Component{};

	UPROPERTY()
	UCharacter_Animation_Instance* Character_Animation_Instance{};
	
	UPROPERTY()
	ACharacter_Direction_Arrow* Character_Direction_Arrow{};

	UPROPERTY()
	USkeletalMeshComponent* Character_Mesh{};


#pragma endregion

#pragma region Inputs

	void On_Player_Enter_Climb_State();
	void On_Player_Exit_Climb_State();
	
	void On_Player_Enter_Take_Cover_State();
	void On_Player_Exit_Take_Cover_State();
	
	void Add_Input_Mapping_Context(UInputMappingContext* Context_To_Add, int32 In_Priority);
	void Remove_Input_Mapping_Context(UInputMappingContext* Context_To_Remove);
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* Climbing_Mapping_Context{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* Take_Cover_Mapping_Context{};

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Parkour_Roll_Action{};

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Jog_Action{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Climbing_Move_Action{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Take_Cover_Move_Action{};

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction{};

	//Parkour, Wall_Run, Climb Inputs & Functions

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Parkour_Action{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Free_Hang_To_Balanced_Walk_Action{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Exit_Parkour_Action{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Crouch_Action{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Turn_In_Place_Action{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Parkour_Slide_Action{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Wall_Run_Action{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Wall_Pipe_Action{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Debug_Action{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Time_Dilation_Action{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Climb_Action{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Climb_Hop_Action{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Take_Cover_Action{};

#pragma endregion

#pragma region Input_Callback
	
	/** Called for movement input */
	void Handle_Ground_Movement_Input_Triggered(const FInputActionValue& Value);

	void Handle_Ground_Movement_Input_Completed(const FInputActionValue& Value);
	
	void Handle_Climb_Movement_Input(const FInputActionValue& Value);

	void Handle_Take_Cover_Movement_Input(const FInputActionValue& Value);

	void On_Parkour_Roll_Started(const FInputActionValue& Value);
	
	void On_Crouch(const FInputActionValue& Value);

	void On_Turn_In_Place(const FInputActionValue& Value);

	void On_Parkour_Slide_Started(const FInputActionValue& Value);
	
	void On_Jogging_Started(const FInputActionValue& Value);

	void On_Jogging_Ended(const FInputActionValue& Value);

	void On_Parkour_Started(const FInputActionValue& Value);

	void On_Free_Hang_To_Balanced_Walk(const FInputActionValue& Value);

	void On_Parkour_Ended(const FInputActionValue& Value);

	void On_Parkour_Ended_Completed(const FInputActionValue& Value);

	void On_Wall_Run_Started(const FInputActionValue& Value);

	void On_Wall_Pipe_Climb_Started(const FInputActionValue& Value);

	void On_Debug_Action(const FInputActionValue& Value);

	void On_Time_Dilation_Started(const FInputActionValue& Value);

	void On_Time_Dilation_Ended(const FInputActionValue& Value);

	void On_Climb_Action_Started(const FInputActionValue& Value);

	void On_Climb_Hop_Action_Started(const FInputActionValue& Value);

	void On_Take_Cover_Action_Started(const FInputActionValue& Value);

	
	/** Called for looking input */
	void Look(const FInputActionValue& Value);

#pragma endregion

#pragma region Variables

	double Up_Down_Look_Value{};

	double Left_Right_Look_Value{};

	UPROPERTY(Replicated)
	bool bIs_Jogging{false};

	int Debug_Selector{};

	UPROPERTY(Replicated)
	bool bDrop_To_Shimmy{false};

	FGameplayTag Canceled_Controller_Direction{};

#pragma endregion

#pragma region Network

#pragma region Character_Network

UFUNCTION(Server, Reliable)
void Server_On_Parkour_Ended_Completed(const FInputActionValue& Value);

#pragma endregion

#pragma region World_Actors_Network

UPROPERTY(ReplicatedUsing = On_Replication_Wall_Pipe_Actor)
AWall_Pipe_Actor* Wall_Pipe_Actor{};

UFUNCTION()
void On_Replication_Wall_Pipe_Actor(AWall_Pipe_Actor* Previous_Wall_Pipe_Actor);

UPROPERTY(ReplicatedUsing = On_Replication_Balance_Traversal_Actor)
ABalance_Traversal_Actor* Balance_Traversal_Actor{};

UFUNCTION()
void On_Replication_Balance_Traversal_Actor(ABalance_Traversal_Actor* Previous_Balance_Traversal_Actor);

UPROPERTY(ReplicatedUsing = On_Replication_Wall_Vault_Actor)
AWall_Vault_Actor* Wall_Vault_Actor{};

UFUNCTION()
void On_Replication_Wall_Vault_Actor(AWall_Vault_Actor* Previous_Wall_Vault_Actor);

UPROPERTY(ReplicatedUsing = On_Replication_Tic_Tac_Actor)
ATic_Tac_Actor* Tic_Tac_Actor{};

UFUNCTION()
void On_Replication_Tic_Tac_Actor(ATic_Tac_Actor* Previous_Tic_Tac_Actor);

UPROPERTY(Replicated)
int Tic_Tac_Actor_Area_Box_ID{};

UPROPERTY(ReplicatedUsing = On_Replication_Stairs_Actor)
AStairs_Actor* Stairs_Actor{};

UFUNCTION()
void On_Replication_Stairs_Actor();


#pragma endregion


#pragma endregion


protected:
	
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float Deltatime);

	virtual void Jump() override;

public:
	
	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* Get_Camera_Boom() const {return Camera_Boom;}
	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* Get_Follow_Camera() const {return Follow_Camera;}
	
	FORCEINLINE UCustom_Movement_Component* Get_Custom_Movement_Component() const {return Custom_Movement_Component;}

	FORCEINLINE UMotionWarpingComponent* Get_Motion_Warping_Component() const {return Motion_Warping_Component;}

	FORCEINLINE bool Set_Is_Jogging(const bool& Is_Character_Jogging) {return bIs_Jogging = Is_Character_Jogging;}

	FORCEINLINE bool Get_Is_Jogging() const {return bIs_Jogging;}

	FORCEINLINE double Get_Up_Down_Look_Value() const {return Up_Down_Look_Value;}

	FORCEINLINE double Get_Left_Right_Look_Value() const {return Left_Right_Look_Value;}

	FORCEINLINE bool Get_bDrop_To_Shimmy() const {return bDrop_To_Shimmy;}

	void Set_Overlapping_Wall_Pipe_Actor(AWall_Pipe_Actor* Overlapping_Wall_Pipe_Actor);

	void Set_Overlapping_Balance_Traversal_Actor(ABalance_Traversal_Actor* Overlapping_Balance_Traversal_Actor);

	void Set_Overlapping_Wall_Vault_Actor(AWall_Vault_Actor* Overlapping_Wall_Vault_Actor);

	void Set_Overlapping_Tic_Tac_Actor(ATic_Tac_Actor* Overlapping_Tic_Tac_Actor, const int& Tic_Tac_Area_Box_ID);

	void Set_Overlapping_Stairs_Actor(AStairs_Actor* Overlapping_Stairs_Actor);

};

