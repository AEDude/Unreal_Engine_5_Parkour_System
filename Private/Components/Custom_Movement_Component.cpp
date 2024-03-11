// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Custom_Movement_Component.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Debug/DebugHelper.h"
#include "Components/CapsuleComponent.h"
#include "Character/Technical_Animator_Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionWarpingComponent.h"
#include "Engine/World.h"
//#include "Gameplay_Tags/Gameplay_Tags.h"
#include "Native_Gameplay_Tags/Native_Gameplay_Tags.h"
#include "DrawDebugHelpers.h"
#include "Character_Direction/Character_Direction_Arrow.h"
#include "Data_Asset/Parkour_Action_Data.h"
#include "Kismet/GameplayStatics.h"

void UCustom_Movement_Component::BeginPlay()
{
	Super::BeginPlay();
	
	Owning_Player_Animation_Instance = CharacterOwner->GetMesh()->GetAnimInstance();

	if(Owning_Player_Animation_Instance)
	{
		Owning_Player_Animation_Instance->OnMontageEnded.AddDynamic(this, &UCustom_Movement_Component::On_Climbing_Montage_Ended);
		Owning_Player_Animation_Instance->OnMontageBlendingOut.AddDynamic(this, &UCustom_Movement_Component::On_Climbing_Montage_Ended);

		Owning_Player_Animation_Instance->OnMontageEnded.AddDynamic(this, &UCustom_Movement_Component::On_Take_Cover_Montage_Ended);
		Owning_Player_Animation_Instance->OnMontageBlendingOut.AddDynamic(this, &UCustom_Movement_Component::On_Take_Cover_Montage_Ended);
	}

	Owning_Player_Character = Cast<ATechnical_Animator_Character>(CharacterOwner);
}

void UCustom_Movement_Component::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const FVector Unrotated_Last_Input_Vector = 
	UKismetMathLibrary::Quat_UnrotateVector(UpdatedComponent->GetComponentQuat(), GetLastInputVector());

	//Debug::Print(Unrotated_Last_Input_Vector.GetSafeNormal().ToCompactString(), FColor:: Cyan, 9);

	Parkour_Call_In_Tick();
}

void UCustom_Movement_Component::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if(Is_Climbing())
	{
		bOrientRotationToMovement = false;
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(48.f);
		On_Enter_Climb_State_Delegate.ExecuteIfBound();
	}

	else if(PreviousMovementMode == MOVE_Custom && PreviousCustomMode == E_Custom_Movement_Mode::MOVE_Climb)
	{
		bOrientRotationToMovement = true;
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(96.f);

		const FRotator Dirty_Rotation = UpdatedComponent->GetComponentRotation();
		const FRotator Clean_Stand_Rotation = FRotator(0.f, Dirty_Rotation.Yaw, 0.f);
		UpdatedComponent->SetRelativeRotation(Clean_Stand_Rotation);

		StopMovementImmediately();

		On_Exit_Climb_State_Delegate.ExecuteIfBound();
	}

	if(Is_Taking_Cover())
	{
		bOrientRotationToMovement = false;
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(48.f);
		CharacterOwner->bIsCrouched = true;
		On_Enter_Take_Cover_State_Delegate.ExecuteIfBound();
	}

	else if(PreviousMovementMode == MOVE_Custom && PreviousCustomMode == E_Custom_Movement_Mode::MOVE_Take_Cover)
	{
		bOrientRotationToMovement = true;
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(96.f);

		const FRotator Dirty_Rotation = UpdatedComponent->GetComponentRotation();
		const FRotator Clean_Stand_Rotation = FRotator(0.f, Dirty_Rotation.Yaw, 0.f);
		UpdatedComponent->SetRelativeRotation(Clean_Stand_Rotation);

		StopMovementImmediately();

		On_Exit_Take_Cover_State_Delegate.ExecuteIfBound();
	}
}

void UCustom_Movement_Component::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	if(Is_Climbing())
	{
		Physics_Climb(deltaTime, Iterations);
	}
	
	if(Is_Taking_Cover())
	{
		Physics_Take_Cover(deltaTime, Iterations);
	}
}

float UCustom_Movement_Component::GetMaxSpeed() const
{
    if(Is_Climbing())
	{
		return Max_Climb_Speed;
	}
	else
	{
		return Super:: GetMaxSpeed();
	}
	
	if(Is_Taking_Cover())
	{
		return Max_Take_Cover_Speed;
	}
	else
	{
		return Super:: GetMaxSpeed();
	}
}

float UCustom_Movement_Component::GetMaxAcceleration() const
{
    if(Is_Climbing())
	{
		return Max_Climb_Acceleration;
	}
	else if(!Is_Climbing())
	{
		return Super:: GetMaxAcceleration();
	}

	if(Is_Taking_Cover())
	{
		return Max_Take_Cover_Acceleration;
	}
	else
	{
		return Super:: GetMaxAcceleration();
	}


	/*if(Is_Climbing() || Is_Taking_Cover())
	{
		if(Is_Climbing())
		{
			return Max_Climb_Acceleration;
		}
		else if(Is_Taking_Cover())
		{
			return Max_Take_Cover_Acceleration;
		}	
	}
	else
	{	
		return Super:: GetMaxAcceleration();
	}*/
}

FVector UCustom_Movement_Component::ConstrainAnimRootMotionVelocity(const FVector &RootMotionVelocity, const FVector &CurrentVelocity) const
{	
	const bool bIs_Playing_RM_Montage =
    IsFalling() && Owning_Player_Animation_Instance && Owning_Player_Animation_Instance->IsAnyMontagePlaying();

	if(bIs_Playing_RM_Montage)
	{
		return RootMotionVelocity;	
	}
	else
	{
		return Super::ConstrainAnimRootMotionVelocity(RootMotionVelocity,CurrentVelocity);
	}
}


#pragma region Climb_Region


#pragma region Climb_Traces

TArray<FHitResult> UCustom_Movement_Component::DoCapsuleTraceMultiByObject(const FVector& Start, const FVector& End, bool B_Show_Debug_Shape, bool bDrawPersistantShapes)
{
	TArray<FHitResult> OutCapsuleTraceHitResults;

	EDrawDebugTrace::Type DebugTraceType = EDrawDebugTrace::None;

	if(B_Show_Debug_Shape)
	{
		DebugTraceType = EDrawDebugTrace::ForOneFrame;

		if(bDrawPersistantShapes)
		{
			DebugTraceType = EDrawDebugTrace::Persistent;
		}
	}
	
	UKismetSystemLibrary::CapsuleTraceMultiForObjects(
		this,
		Start,
		End,
		Climb_Capsule_Trace_Radius,
		Climb_Capsule_Trace_Half_Height,
		Climable_Surface_Trace_Types,
		false,
		TArray<AActor*>(),
		DebugTraceType,
		OutCapsuleTraceHitResults,
		false
	);

	return OutCapsuleTraceHitResults;
}

FHitResult UCustom_Movement_Component::Do_Line_Trace_Single_By_Object(const FVector& Start, const FVector& End, bool B_Show_Debug_Shape, bool bDrawPersistantShapes)
{
	FHitResult OutHit;

	EDrawDebugTrace::Type DebugTraceType = EDrawDebugTrace::None;

	if(B_Show_Debug_Shape)
	{
		DebugTraceType = EDrawDebugTrace::ForOneFrame;

		if(bDrawPersistantShapes)
		{
			DebugTraceType = EDrawDebugTrace::Persistent;
		}
	}
	
	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		Start,
		End,
		Climable_Surface_Trace_Types,
		false,
		TArray<AActor*>(),
		DebugTraceType,
		OutHit,
		false
	);

		return OutHit;
}
#pragma endregion

#pragma region Climb_Core

void UCustom_Movement_Component::Toggle_Climbing(bool B_Eneble_Climb)
{
	if(B_Eneble_Climb)
	{
		if(Can_Start_Climbing())
		{
			//Enter Climb The State
			Debug::Print(TEXT("Can Start Climbing"));
			Play_Climbing_Montage(Idle_To_Climb_Montage);
		}
		else if (Can_Climb_Down_Ledge())
		{
			Play_Climbing_Montage(Climb_Down_Ledge_Montage);
			Debug::Print(TEXT("Can Climb Down"), FColor::Cyan, 4);
		}
		else
		{
			Try_Start_Vaulting();
			Debug::Print(TEXT("Can't Climb Down"), FColor::Green, 8);
		}
	}
	if(!B_Eneble_Climb)
	{
		//Stop Climbing
		Stop_Climbing();
	}
}

bool UCustom_Movement_Component::Can_Start_Climbing()
{
    if(IsFalling()) return false;
	if(!Trace_Climbable_Surfaces()) return false;
	if(!Trace_From_Eye_Height(100.f).bBlockingHit) return false;

	return true;
}

bool UCustom_Movement_Component::Can_Climb_Down_Ledge()
{
    if(IsFalling()) return false;

	const FVector Component_Loation = UpdatedComponent->GetComponentLocation();
	const FVector Component_Forward_Vector = UpdatedComponent->GetForwardVector();
	const FVector Component_Down_Vector = -UpdatedComponent->GetUpVector();

	const FVector Walkable_Surface_Trace_Start = Component_Loation + Component_Forward_Vector * Climb_Down_Walkable_Surface_Trace_Offset;
	const FVector Walkable_Surface_Trace_End = Walkable_Surface_Trace_Start + Component_Down_Vector * 100.f;

	FHitResult Walkable_Surface_Hit = Do_Line_Trace_Single_By_Object(Walkable_Surface_Trace_Start, Walkable_Surface_Trace_End, 1, 0);

	const FVector Ledge_Trace_Start = Walkable_Surface_Hit.TraceStart + Component_Forward_Vector * Climb_Down_Ledge_Trace_Offset;
	const FVector Ledge_Trace_End = Ledge_Trace_Start + Component_Down_Vector * 230.f;

	FHitResult Ledge_Trace_Hit = Do_Line_Trace_Single_By_Object(Ledge_Trace_Start, Ledge_Trace_End, 1, 0);

	if(Walkable_Surface_Hit.bBlockingHit && !Ledge_Trace_Hit.bBlockingHit)
	{
		return true;
	}
	/*if(!Walkable_Surface_Hit.bBlockingHit && !Ledge_Trace_Hit.bBlockingHit)
	{
		return true;
	}*/
	return false;
}

void UCustom_Movement_Component::Start_Climbing()
{
	SetMovementMode(MOVE_Custom, E_Custom_Movement_Mode::MOVE_Climb);
}

void UCustom_Movement_Component::Stop_Climbing()
{
	SetMovementMode(MOVE_Falling);
}

void UCustom_Movement_Component::Physics_Climb(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	/*Process all climable surfaces information*/

	Trace_Climbable_Surfaces();

	Process_Climbable_Surface_Info();

	if(Check_Has_Reached_Floor())
	{
		Debug::Print(TEXT("Has Reached Floor"), FColor::Yellow, 5);
	}
	else
	{
		Debug::Print(TEXT("Floor Not Reached"), FColor::Green, 7);
	}

	/*Check if we should stop climbing*/
	if(Check_Should_Stop_Climbing() || Check_Has_Reached_Floor())
	{
		Stop_Climbing();
	}


	RestorePreAdditiveRootMotionVelocity();

	if( !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() )
	{	/*Define the max climb speed and acceleration*/
		CalcVelocity(deltaTime, 0.f, true, Max_Climb_Speed);
	}

	ApplyRootMotionToVelocity(deltaTime);


	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FVector Adjusted = Velocity * deltaTime;
	FHitResult Hit(1.f);
	
	/*Handle the climbing rotation*/
	SafeMoveUpdatedComponent(Adjusted, Get_Climb_Rotation(deltaTime), true, Hit);

	if (Hit.Time < 1.f)
	{
		//adjust and try again
			HandleImpact(Hit, deltaTime, Adjusted);
			SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}

	if(!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() )
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}

	/*Snap movement to climable surfaces*/
	Snap_Movement_To_Climbable_Surfaces(deltaTime);

	if(Check_Has_Has_Reached_Ledge())
	{	
		Play_Climbing_Montage(Climb_To_Top_Montage);
		Debug::Print(TEXT("Top of Surface Reached"), FColor::Yellow, 3);
	}
	else
	{
		Debug::Print(TEXT("Top of Surface Not Reached"), FColor::Emerald, 4);
	}
}

void UCustom_Movement_Component::Process_Climbable_Surface_Info()
{
	Current_Climbable_Surface_Location = FVector::ZeroVector;
	Current_Climbable_Surface_Normal = FVector::ZeroVector;

	if(Climable_Surfaces_Traced_Results.IsEmpty()) return;

	for(const FHitResult& TracedHitResult:Climable_Surfaces_Traced_Results)
	{
		Current_Climbable_Surface_Location += TracedHitResult.ImpactPoint;
		Current_Climbable_Surface_Normal += TracedHitResult.ImpactNormal;
	}

	Current_Climbable_Surface_Location /= Climable_Surfaces_Traced_Results.Num();
	Current_Climbable_Surface_Normal = Current_Climbable_Surface_Normal.GetSafeNormal();

	Debug::Print(TEXT("Climbable_Surface_Location:") + Current_Climbable_Surface_Location.ToCompactString(), FColor::Cyan, 1);
	Debug::Print(TEXT("Climbable_Surface_Normal:") + Current_Climbable_Surface_Normal.ToCompactString(), FColor::Red, 2);
}

bool UCustom_Movement_Component::Check_Should_Stop_Climbing()
{
    if(Climable_Surfaces_Traced_Results.IsEmpty()) return true;

	const float Dot_Result = FVector::DotProduct(Current_Climbable_Surface_Normal, FVector::UpVector);
	const float Degree_Difference = FMath::RadiansToDegrees(FMath::Acos(Dot_Result));

	if(Degree_Difference <= 59.f)
	{
		return true;
	}

	Debug::Print(TEXT("Degree Difference:") + FString::SanitizeFloat(Degree_Difference), FColor::Cyan, 1);

	return false;
}

bool UCustom_Movement_Component::Check_Has_Reached_Floor()
{
    const FVector Down_Vector = -UpdatedComponent->GetUpVector();
	const FVector Start_Offset = Down_Vector * 50.f;

	const FVector Start = UpdatedComponent->GetComponentLocation() + Start_Offset;
	const FVector End = Start + Down_Vector;

	TArray<FHitResult> Possible_Floor_Hits = DoCapsuleTraceMultiByObject(Start, End, 0, 0);

	if(Possible_Floor_Hits.IsEmpty()) return false;

	for(const FHitResult& Possible_Floor_Hit:Possible_Floor_Hits)
	{
		const bool bFloorReached = 
		FVector::Parallel(-Possible_Floor_Hit.ImpactNormal, FVector::UpVector) &&
		Get_Unrotated_Climb_Velocity().Z<-10.f;

		if(bFloorReached)
		{
			return true;
		}
	}
	return false;
}

FQuat UCustom_Movement_Component::Get_Climb_Rotation(float DeltaTime)
{
    const FQuat Current_Quat = UpdatedComponent->GetComponentQuat();
	
	if(HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity())
	{
		return Current_Quat;
	}
	const FQuat Target_Quat = FRotationMatrix::MakeFromX(-Current_Climbable_Surface_Normal).ToQuat();

	return FMath::QInterpTo(Current_Quat, Target_Quat, DeltaTime, 5.f);
	
}

void UCustom_Movement_Component::Snap_Movement_To_Climbable_Surfaces(float DeltaTime)
{
	const FVector Component_Forward = UpdatedComponent->GetForwardVector();
	const FVector Component_Location = UpdatedComponent->GetComponentLocation();

	const FVector Projected_Character_To_Surface = 
	(Current_Climbable_Surface_Location - Component_Location).ProjectOnTo(Component_Forward);

	const FVector Snap_Vector = -Current_Climbable_Surface_Normal * Projected_Character_To_Surface.Length();

	UpdatedComponent->MoveComponent(
	Snap_Vector * DeltaTime * Max_Climb_Speed,
	UpdatedComponent->GetComponentQuat(),
	true);
}

bool UCustom_Movement_Component::Check_Has_Has_Reached_Ledge()
{
	FHitResult Ledge_Hit_Result = Trace_From_Eye_Height(100.f, 50.f);

	if(!Ledge_Hit_Result.bBlockingHit)
	{
		const FVector Walkable_Surface_Trace_Start = Ledge_Hit_Result.TraceEnd;
		
		const FVector Down_Vector = -UpdatedComponent->GetUpVector();

		const FVector Walkable_Surface_Trace_End = Walkable_Surface_Trace_Start + Down_Vector * 100.f;
		
		FHitResult Walkable_Surface_Hit_Result =
		Do_Line_Trace_Single_By_Object(Walkable_Surface_Trace_Start, Walkable_Surface_Trace_End, 1, 0);

		if(Walkable_Surface_Hit_Result.bBlockingHit && Get_Unrotated_Climb_Velocity().Z > 10.f)
		{
			return true;
			//Use dot product to check the surface normal and or check if it's parallel with up vector.
		}
	}
	return false;
}

void UCustom_Movement_Component::Try_Start_Vaulting()
{
	FVector Vault_Start_Position;
	FVector Vault_End_Position;
	if(Can_Start_Vaulting(Vault_Start_Position, Vault_End_Position))
	{
		//Start Vaulting
		Set_Motion_Warping_Target(FName("Vault_Start_Position"), Vault_Start_Position);
		Set_Motion_Warping_Target(FName("Vault_End_Position"), Vault_End_Position);
		Debug::Print(TEXT("Vault Start position:") + Vault_Start_Position.ToCompactString());
		Debug::Print(TEXT("ault_End_Position:") + Vault_End_Position.ToCompactString());

		Start_Climbing();
		Play_Climbing_Montage(Vaulting_Montage);
	}
	else
	{
		Debug::Print(TEXT("Unable To Vault"));
	}
}

bool UCustom_Movement_Component::Can_Start_Vaulting(FVector& Out_Vault_Start_Position, FVector& Out_Vault_Land_Position)
{
    if(IsFalling()) return false;
	Out_Vault_Start_Position = FVector::ZeroVector;
	Out_Vault_Land_Position = FVector::ZeroVector;
	const FVector Component_Location = UpdatedComponent->GetComponentLocation();
	const FVector Component_Forward_Vector = UpdatedComponent->GetForwardVector();
	const FVector Component_Up_Vector = UpdatedComponent->GetUpVector();
	const FVector Component_Down_Vector = -UpdatedComponent->GetUpVector();

	for(int32 i = 0; i < 5; i++)
	{
		const FVector Start = Component_Location + Component_Up_Vector * 100.f + 
		Component_Forward_Vector * 100.f * (i+1);

		const FVector End = Start + Component_Down_Vector * 100.f * (i+1);

		FHitResult Vault_Trace_Hit = Do_Line_Trace_Single_By_Object(Start, End, 1, 0);

		if(i == 0 && Vault_Trace_Hit.bBlockingHit)
		{
			Out_Vault_Start_Position = Vault_Trace_Hit.ImpactPoint;
		}
		if(i == 4)
		{
			Out_Vault_Land_Position = Vault_Trace_Hit.ImpactPoint;
		}
	}
	if(Out_Vault_Start_Position != FVector::ZeroVector && Out_Vault_Land_Position != FVector::ZeroVector)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool UCustom_Movement_Component::Is_Climbing() const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == E_Custom_Movement_Mode::MOVE_Climb;
}

//Trace for climable surfaces return true if there are valid surfaces, otherwise return false.
bool UCustom_Movement_Component::Trace_Climbable_Surfaces()
{
	const FVector Start_Offset = UpdatedComponent->GetForwardVector() * 30.f;
	
	const FVector Start = UpdatedComponent->GetComponentLocation() + Start_Offset;
	
	const FVector End = Start + UpdatedComponent->GetForwardVector();
	
	Climable_Surfaces_Traced_Results = DoCapsuleTraceMultiByObject(Start, End, 0);

	return !Climable_Surfaces_Traced_Results.IsEmpty();
}

FHitResult UCustom_Movement_Component::Trace_From_Eye_Height(float Trace_Distance, float Trace_Start_Offset, bool B_Show_Debug_Shape, bool bDrawPersistantShapes)
{
	const FVector Component_Location = UpdatedComponent->GetComponentLocation();
	
	const FVector Eye_Height_Offset = UpdatedComponent->GetUpVector() * (CharacterOwner->BaseEyeHeight + Trace_Start_Offset);
	
	const FVector Start = Component_Location + Eye_Height_Offset;
	
	const FVector End = Start + UpdatedComponent->GetForwardVector() * Trace_Distance;

	return Do_Line_Trace_Single_By_Object(Start, End, B_Show_Debug_Shape, bDrawPersistantShapes);
}

void UCustom_Movement_Component::Play_Climbing_Montage(UAnimMontage *MontageToPlay)
{
	if(!MontageToPlay) return;
	if(!Owning_Player_Animation_Instance) return;
	if(Owning_Player_Animation_Instance->IsAnyMontagePlaying()) return;

	Owning_Player_Animation_Instance->Montage_Play(MontageToPlay);
}

void UCustom_Movement_Component::On_Climbing_Montage_Ended(UAnimMontage* Montage, bool bInterrupted)
{
	if(Montage == Idle_To_Climb_Montage || Montage == Climb_Down_Ledge_Montage)
	{
		Start_Climbing();
		StopMovementImmediately();
	}
	if (Montage == Climb_To_Top_Montage || Montage == Vaulting_Montage)
	{
		SetMovementMode(MOVE_Walking);
	}
	
	return Debug::Print(TEXT("Montage is Working"));
}

void UCustom_Movement_Component::Request_Hopping()
{
	const FVector Unrotated_Last_Input_Vector = 
	UKismetMathLibrary::Quat_UnrotateVector(UpdatedComponent->GetComponentQuat(), GetLastInputVector());

	const float Dot_Product_Result = 
	FVector::DotProduct(Unrotated_Last_Input_Vector.GetSafeNormal(), FVector::UpVector);

	const FVector Component_Velocity = UpdatedComponent->GetComponentVelocity();
	const FVector Right_Vector = UpdatedComponent->GetRightVector();
	 
	const float Dot_Product_Result_2 = FVector::DotProduct(Velocity.GetSafeNormal(), Right_Vector); 

	Debug::Print(TEXT("Dot Product Result") + FString::SanitizeFloat(Dot_Product_Result));

	if(Dot_Product_Result > 0.9f)
	{
		Handle_Hop_Hop();
		Debug::Print(TEXT("Hopping Up"));
	}
	else if(Dot_Product_Result <= -0.9f)
	{
		Handle_Hop_Down();
		Debug::Print(TEXT("Hopping Down"));
	}
	if(Dot_Product_Result_2 > 0.9f)
	{
		Handle_Hop_Right();
		Debug::Print(TEXT("Hopping Right"));
	}
	else if(Dot_Product_Result_2 <= -0.9f)
	{
		Handle_Hop_Left();
		Debug::Print(TEXT("Hopping Left"));
	}
	else
	{
		Debug::Print(TEXT("Desired Directional Hopping Has Yet To Be Implemented"));
	}
}

void UCustom_Movement_Component::Set_Motion_Warping_Target(const FName &In_Warping_Target_Name, const FVector &In_Target_Position)
{
	if(!Owning_Player_Character) return;

	Owning_Player_Character->Get_Motion_Warping_Component()->AddOrUpdateWarpTargetFromLocation(
		In_Warping_Target_Name,
		In_Target_Position
	);
}

void UCustom_Movement_Component::Handle_Hop_Hop()
{
	FVector Hop_Up_Target_Point;
	if(bCheck_Can_Hop_Up(Hop_Up_Target_Point))
	{
		Set_Motion_Warping_Target(FName("Hop_Up_Target_Point"), Hop_Up_Target_Point);
		
		Play_Climbing_Montage(Hop_Up_Montage);
		
		Debug::Print(TEXT("Hopping Up"));
	}
	
}

bool UCustom_Movement_Component::bCheck_Can_Hop_Up(FVector& Out_Hop_Up_Target_Position)
{
    FHitResult Hop_Up_Hit = Trace_From_Eye_Height(100.f, -15.f, true, true);
	FHitResult Safety_Ledge_Hit = Trace_From_Eye_Height(100.f, 150.f, true, true);

	if(Hop_Up_Hit.bBlockingHit && Safety_Ledge_Hit.bBlockingHit)
	{
		Out_Hop_Up_Target_Position = Hop_Up_Hit.ImpactPoint;
		return true;
	}
	return false;
}

void UCustom_Movement_Component::Handle_Hop_Down()
{
	FVector Hop_Down_Target_Point;
	if(bCheck_Can_Hop_Down(Hop_Down_Target_Point))
	{
		Set_Motion_Warping_Target(FName("Hop_Down_Target_Point"), Hop_Down_Target_Point);


		Play_Climbing_Montage(Hop_Down_Montage);
	}
	else
	{
		Debug::Print(TEXT("Can't Hop Down"));
	}
}

bool UCustom_Movement_Component::bCheck_Can_Hop_Down(FVector &Out_Hop_Down_Target_Point)
{	
	float Offset = 300.f;
	FVector Location = UpdatedComponent->GetComponentLocation();
	FVector Down_Vector = -UpdatedComponent->GetUpVector();
	FVector Forward_Vector = UpdatedComponent->GetForwardVector();
	
	FVector Start = Location + Forward_Vector * 2.f;
	FVector End = Start + Down_Vector * Offset;

	FHitResult Ground_Safety_Check = Do_Line_Trace_Single_By_Object(Start, End, 1, 1);
	FHitResult Hop_Down_Wall_Hit = Trace_From_Eye_Height(100.f, -300.f, 1, 1);
	
	if(Ground_Safety_Check.bBlockingHit)
	{
		return false;
	}
	else if (!Ground_Safety_Check.bBlockingHit && Hop_Down_Wall_Hit.bBlockingHit)
	{
		Out_Hop_Down_Target_Point = Hop_Down_Wall_Hit.ImpactPoint;
		return true;
	}
	return false;
	
	/*FHitResult Hop_Down_Wall_Hit = Trace_From_Eye_Height(100.f, -300.f);

	if(Hop_Down_Wall_Hit.bBlockingHit)
	{
		Out_Hop_Down_Target_Point = Hop_Down_Wall_Hit.ImpactPoint;
		return true;
	}
	return false; */	
}

void UCustom_Movement_Component::Handle_Hop_Left()
{
	FVector Hop_Left_Target_Point;
	if(bCheck_Can_Hop_Left(Hop_Left_Target_Point))
	{
		Set_Motion_Warping_Target(FName("Hop_Left_Target_Point"), Hop_Left_Target_Point);

		Play_Climbing_Montage(Hop_Left_Montage);
	}
	else
	{
		Debug::Print(TEXT("Can't Hop Left"));
	}

}

bool UCustom_Movement_Component::bCheck_Can_Hop_Left(FVector &Out_Hop_Left_Target_Point)
{
	float Offset = 100.f;
    FVector Location = UpdatedComponent->GetComponentLocation();
	FVector Forward_Vector = UpdatedComponent->GetForwardVector();
	FVector Component_Left_Vector = -UpdatedComponent->GetRightVector();
	FVector Component_Down_Vector = -UpdatedComponent->GetUpVector();
	FVector Down_Point = Location + Component_Down_Vector * Offset;
	FVector Left_Point = Down_Point + Component_Left_Vector * Climbing_Hop_Trace_Offset;
	FVector Wall_Point = Left_Point + Forward_Vector * Offset;
	
	FHitResult Wall_Safety_Check = Do_Line_Trace_Single_By_Object(Left_Point, Wall_Point, true, true);

	if(Wall_Safety_Check.bBlockingHit)
	{
		Out_Hop_Left_Target_Point = Wall_Safety_Check.ImpactPoint;
		return true;
	}
	return false;
}

void UCustom_Movement_Component::Handle_Hop_Right()
{
	FVector Hop_Right_Target_Point;
	if(bCheck_Can_Hop_Right(Hop_Right_Target_Point))
	{
		Set_Motion_Warping_Target(FName("Hop_Right_Target_Point"), Hop_Right_Target_Point);

		Play_Climbing_Montage(Hop_Right_Montage);
	}
	else
	{
		Debug::Print(TEXT("Can't Hop Right"));
	}
}

bool UCustom_Movement_Component::bCheck_Can_Hop_Right(FVector &Out_Hop_Right_Target_Point)
{
    float Offset = 100.f;
	FVector Location = UpdatedComponent->GetComponentLocation();
	FVector Forward_Vector = UpdatedComponent->GetForwardVector();
	FVector Right_Vector = UpdatedComponent->GetRightVector();
	FVector Down_Vector = -UpdatedComponent->GetUpVector();
	FVector Down_Point = Location + Down_Vector * Offset;
	FVector Right_Point = Down_Point + Right_Vector * Climbing_Hop_Trace_Offset;
	FVector Wall_Point = Right_Point + Forward_Vector * Offset;

	FHitResult Wall_Safety_Check = Do_Line_Trace_Single_By_Object(Right_Point, Wall_Point, true, true);

	if(Wall_Safety_Check.bBlockingHit)
	{
		Out_Hop_Right_Target_Point = Wall_Safety_Check.ImpactPoint;
		return true;
	}
	return false;
}

FVector UCustom_Movement_Component::Get_Unrotated_Climb_Velocity() const
{
    return UKismetMathLibrary::Quat_UnrotateVector(UpdatedComponent->GetComponentQuat(), Velocity);
}

#pragma endregion


#pragma endregion



#pragma region Take_Cover_Region


#pragma region Take_Cover_Traces

TArray<FHitResult> UCustom_Movement_Component::Do_Capsule_Trace_Multi_By_Object_Take_Cover(const FVector& Start, const FVector& End, bool B_Show_Debug_Shape, bool B_Draw_Persistant_Shapes)
{
	TArray<FHitResult> Out_Capsule_Trace_Hit_Results;

	EDrawDebugTrace::Type Debug_Trace_Type = EDrawDebugTrace::None;

	if(B_Show_Debug_Shape)
	{
		Debug_Trace_Type = EDrawDebugTrace::ForOneFrame;

		if(B_Draw_Persistant_Shapes)
		{
			Debug_Trace_Type = EDrawDebugTrace::Persistent;
		}
	}

	UKismetSystemLibrary::CapsuleTraceMultiForObjects(
		this,
		Start,
		End,
		Take_Cover_Capsule_Trace_Radius,
		Take_Cover_Capsule_Trace_Half_Height,
		Take_Cover_Surface_Trace_Types,
		false,
		TArray<AActor*>(),
		Debug_Trace_Type,
		Out_Capsule_Trace_Hit_Results,
		false
	);
	
	return Out_Capsule_Trace_Hit_Results;
}

FHitResult UCustom_Movement_Component::Do_Line_Trace_Single_By_Object_Take_Cover(const FVector& Start, const FVector& End, bool B_Show_Debug_Shape, bool B_Draw_Persistant_Shapes)
{
	FHitResult Out_Hit;
	
	EDrawDebugTrace::Type Debug_Trace_Type = EDrawDebugTrace::None;

	if(B_Show_Debug_Shape)
	{
		Debug_Trace_Type = EDrawDebugTrace::ForOneFrame;

		if(B_Draw_Persistant_Shapes)
		{
			Debug_Trace_Type = EDrawDebugTrace::Persistent;
		}
	}

	/*GetWorld()->LineTraceSingleByChannel(
		Out_Hit,
		Start,
		End,
		ECollisionChannel::ECC_GameTraceChannel1
	);*/

	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		Start,
		End,
		Take_Cover_Surface_Trace_Types,
		false,
		TArray<AActor*>(),
		Debug_Trace_Type,
		Out_Hit,
		false
	);

	return Out_Hit;
}

FHitResult UCustom_Movement_Component::Do_Sphere_Trace_For_Objects(const FVector& Start, const FVector& End, bool B_Show_Debug_Shape, bool B_Draw_Persistent_Shapes)
{
	FHitResult Out_Hit;
	
	EDrawDebugTrace::Type Debug_Trace_Type = EDrawDebugTrace::None;

	if(B_Show_Debug_Shape)
	{
		Debug_Trace_Type = EDrawDebugTrace::ForOneFrame;

		if(B_Draw_Persistent_Shapes)
		{
			Debug_Trace_Type = EDrawDebugTrace::Persistent;
		}
	}

	UKismetSystemLibrary::SphereTraceSingleForObjects(
		this,
		Start,
		End,
		Take_Cover_Sphere_Trace_Radius,
		Take_Cover_Surface_Trace_Types,
		false,
		TArray<AActor*>(),
		Debug_Trace_Type,
		Out_Hit,
		false
		);
	
	return Out_Hit;

}

#pragma endregion

#pragma region Take_Cover_Traces_Implemented

//Trace for surfaces which player can take cover. Return true if there are valid surfaces, otherwise returns false.
bool UCustom_Movement_Component::Capsule_Trace_Take_Cover_Surfaces()
{
	const FVector Start_Offset = UpdatedComponent->GetForwardVector() * 55.f;
	const FVector Start = UpdatedComponent->GetComponentLocation() + Start_Offset;
	const FVector End = Start + UpdatedComponent->GetForwardVector();

	Take_Cover_Surfaces_Traced_Results = Do_Capsule_Trace_Multi_By_Object_Take_Cover(Start, End, true /*,true */);

	return !Take_Cover_Surfaces_Traced_Results.IsEmpty();
}

bool UCustom_Movement_Component::Capsule_Trace_Ground_Surface()
{
	const FVector Start_Offset = -UpdatedComponent->GetUpVector() * 30.f;
	const FVector Start = UpdatedComponent->GetComponentLocation() + Start_Offset;
	const FVector End = Start + -UpdatedComponent->GetForwardVector();

	Take_Cover_Ground_Surface_Traced_Results = Do_Capsule_Trace_Multi_By_Object_Take_Cover(Start, End, true /*, true*/);

	return !Take_Cover_Ground_Surface_Traced_Results.IsEmpty();
}

FHitResult UCustom_Movement_Component::Sphere_Trace_Trace_Take_Cover()
{
	const FVector Forward = UpdatedComponent->GetForwardVector();

	const FVector Start = UpdatedComponent->GetComponentLocation();
	const FVector End = Start + Forward * 200.f;
	
	 return Sphere_Trace_Hit_Result = Do_Sphere_Trace_For_Objects(Start, End, true, false);
}

FHitResult UCustom_Movement_Component::Line_Trace_Check_Cover_Right(float Trace_Distance, float Trace_Start_Offset)
{
	const FVector Component_Location = UpdatedComponent->GetComponentLocation();
	const FVector Right_Offset = UpdatedComponent->GetRightVector() * Trace_Start_Offset;
	
	const FVector Start = Component_Location + Right_Offset;
	const FVector End = Start + UpdatedComponent->GetForwardVector() * Trace_Distance;

 	return Do_Line_Trace_Single_By_Object_Take_Cover(Start, End /*, true, true*/);
}

FHitResult UCustom_Movement_Component::Line_Trace_Check_Cover_Left(float Trace_Distance, float Trace_Start_Offset)
{
	const FVector Component_Location = UpdatedComponent->GetComponentLocation();
	const FVector Left_Offset = -UpdatedComponent->GetRightVector() * Trace_Start_Offset;
	
	const FVector Start = Component_Location + Left_Offset;
	const FVector End = Start + UpdatedComponent->GetForwardVector() * Trace_Distance;

 	return Do_Line_Trace_Single_By_Object_Take_Cover(Start, End /*, true, true*/);
}

#pragma endregion

#pragma region Take_Cover_Core

void UCustom_Movement_Component::Toggle_Take_Cover(bool bEneble_Take_Cover)
{
	if(bEneble_Take_Cover)
	{
		FVector Take_Cover_End_Position;
		
		if(Can_Take_Cover(Take_Cover_End_Position))
		{
			//Enter Take Cover State
			Debug::Print(TEXT("Can Take Cover!"));
			Debug::Print(TEXT("Take Cover End Position: ") + Take_Cover_End_Position.ToCompactString());
			Set_Motion_Warping_Target(FName("Take_Cover_Crouch"), Take_Cover_End_Position);
			
			Play_Take_Cover_Montage(Idle_To_Take_Cover_Montage);
		}
		else
		{
			Debug::Print(TEXT("Can't Take Cover!"));
		}
	}
	else
	{
		//Stop Taking Cover
		Play_Take_Cover_Montage(Exit_Cover_To_Stand);
	}
}

bool UCustom_Movement_Component::Can_Take_Cover(FVector& Out_Take_Cover_End_Position)
{
	Sphere_Trace_Trace_Take_Cover();

	Out_Take_Cover_End_Position = FVector::ZeroVector;

	if(IsFalling() || !Sphere_Trace_Hit_Result.bBlockingHit) return false;
	if(!Capsule_Trace_Take_Cover_Surfaces() || !Capsule_Trace_Ground_Surface()) return false;
	if(!Line_Trace_Check_Cover_Right(Take_Cover_Check_Cover_Edge).bBlockingHit || !Line_Trace_Check_Cover_Right(Take_Cover_Check_Cover_Edge).bBlockingHit) return false;

	if(Sphere_Trace_Hit_Result.bBlockingHit)
	{
		Out_Take_Cover_End_Position = Sphere_Trace_Hit_Result.ImpactPoint;
	}

	if(Out_Take_Cover_End_Position != FVector::ZeroVector)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void UCustom_Movement_Component::Start_Take_Cover()
{
	SetMovementMode(MOVE_Custom, E_Custom_Movement_Mode::MOVE_Take_Cover);
}

void UCustom_Movement_Component::Stop_Take_Cover()
{
	SetMovementMode(MOVE_Walking);
}

void UCustom_Movement_Component::Physics_Take_Cover(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	//Process all take cover surfaces information

	Capsule_Trace_Take_Cover_Surfaces();

	Capsule_Trace_Ground_Surface();

	Process_Take_Cover_Surface_Info();

	Process_Take_Cover_Ground_Surface_Info();

	/* Check if we should exit taking cover*/
	if(Check_Should_Exit_Take_Cover())
	{
		Stop_Take_Cover();
	}

	RestorePreAdditiveRootMotionVelocity();

	if( !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() )
	{	
		/*Define the max take cover speed and acceleration*/
		CalcVelocity(deltaTime, 0.f, true, Max_Brake_Take_Cover_Deceleration);
	}

	ApplyRootMotionToVelocity(deltaTime);


	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FVector Adjusted = Velocity * deltaTime;
	FHitResult Hit(1.f);
	
	/*Handle the take cover rotation*/
	SafeMoveUpdatedComponent(Adjusted, Get_Take_Cover_Rotation(deltaTime), true, Hit);

	if (Hit.Time < 1.f)
	{
		//adjust and try again
			HandleImpact(Hit, deltaTime, Adjusted);
			SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}

	if(!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() )
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}

	/*Snap movement to take cover surfaces*/
	Snap_Movement_To_Take_Cover_Surfaces(deltaTime);
	Take_Cover_Snap_Movement_To_Ground(deltaTime);
}

void UCustom_Movement_Component::Process_Take_Cover_Surface_Info()
{
	Current_Take_Cover_Surface_Location = FVector::ZeroVector;
	Current_Take_Cover_Surface_Normal = FVector::ZeroVector;

	if(Take_Cover_Surfaces_Traced_Results.IsEmpty()) return;

	for(const FHitResult& Traced_Hit_Result : Take_Cover_Surfaces_Traced_Results)
	{
		Current_Take_Cover_Surface_Location += Traced_Hit_Result.ImpactPoint;
		Current_Take_Cover_Surface_Normal += Traced_Hit_Result.ImpactNormal;
	}

	Current_Take_Cover_Surface_Location /= Take_Cover_Surfaces_Traced_Results.Num();
	Current_Take_Cover_Surface_Normal = Current_Take_Cover_Surface_Normal.GetSafeNormal();
}

void UCustom_Movement_Component::Process_Take_Cover_Ground_Surface_Info()
{
	if(Take_Cover_Ground_Surface_Traced_Results.IsEmpty()) return;

	for(const FHitResult& Traced_Hit_Result : Take_Cover_Ground_Surface_Traced_Results)
	{
		Current_Take_Cover_Ground_Surface_Location += Traced_Hit_Result.ImpactPoint;
		Current_Take_Cover_Ground_Surface_Normal += Traced_Hit_Result.ImpactNormal;
	}

	Current_Take_Cover_Ground_Surface_Location /= Take_Cover_Ground_Surface_Traced_Results.Num();
	Current_Take_Cover_Ground_Surface_Normal = Current_Take_Cover_Ground_Surface_Normal.GetSafeNormal();

	Debug::Print(TEXT("Take Cover Ground Surface Location: ") + Current_Take_Cover_Ground_Surface_Location.ToCompactString(), FColor::Green, 1);
	Debug::Print(TEXT("Take Cover Ground Surface Normal: ") + Current_Take_Cover_Ground_Surface_Normal.ToCompactString(), FColor::Yellow, 2);
}

bool UCustom_Movement_Component::Check_Should_Exit_Take_Cover()
{
	if(Take_Cover_Surfaces_Traced_Results.IsEmpty()) return true;

	const float Dot_Result = FVector::DotProduct(Current_Take_Cover_Surface_Normal, FVector::UpVector);
	const float Degree_Difference = FMath::RadiansToDegrees(FMath::Acos(Dot_Result));

	if(Degree_Difference <= 87.f)
	{
		return true;
	}

	Debug::Print(TEXT("Take Cover Edge Degree Difference") + FString::SanitizeFloat(Degree_Difference), FColor::Yellow, 1);

	return false;
}

FQuat UCustom_Movement_Component::Get_Take_Cover_Rotation(float DeltaTime)
{
	const FQuat Current_Quat = UpdatedComponent->GetComponentQuat();

	if(HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity())
	{
		return Current_Quat;
	}

	const FQuat Target_Quat = FRotationMatrix::MakeFromX(-Current_Take_Cover_Surface_Normal).ToQuat();
	
	return FMath::QInterpTo(Current_Quat, Target_Quat, DeltaTime, 5.f);
}

void UCustom_Movement_Component::Snap_Movement_To_Take_Cover_Surfaces(float DeltaTime)
{
	const FVector Component_Forward = UpdatedComponent->GetForwardVector();
	const FVector Component_Locataion = UpdatedComponent->GetComponentLocation();

	const FVector Projected_Character_To_Surface = 
	(Current_Take_Cover_Surface_Location - Component_Locataion).ProjectOnTo(Component_Forward);

	const FVector Snap_Vector = -Current_Take_Cover_Surface_Normal * Projected_Character_To_Surface.Length();

	UpdatedComponent->MoveComponent
	(Snap_Vector * DeltaTime * Max_Take_Cover_Speed,
	UpdatedComponent->GetComponentQuat(),
	true
	);
}

void UCustom_Movement_Component::Take_Cover_Snap_Movement_To_Ground(float DeltaTime)
{
	const FVector Component_Down = -UpdatedComponent->GetUpVector();
	const FVector Component_Locataion = UpdatedComponent->GetComponentLocation();

	const FVector Projected_Character_To_Surface = 
	(Current_Take_Cover_Ground_Surface_Location - Component_Locataion).ProjectOnTo(Component_Down);

	const FVector Snap_Vector = -Current_Take_Cover_Ground_Surface_Normal * Projected_Character_To_Surface.Length();

	UpdatedComponent->MoveComponent
	(Snap_Vector * DeltaTime * Max_Take_Cover_Speed,
	UpdatedComponent->GetComponentQuat(),
	true
	);
}

void UCustom_Movement_Component::Play_Take_Cover_Montage(UAnimMontage* Montage_To_Play)
{
	if(!Montage_To_Play) return;
	if(!Owning_Player_Animation_Instance) return;
	if(Owning_Player_Animation_Instance->IsAnyMontagePlaying()) return;

	Owning_Player_Animation_Instance->Montage_Play(Montage_To_Play);
}

void UCustom_Movement_Component::On_Take_Cover_Montage_Ended(UAnimMontage* Montage, bool bInterrupted)
{
	if(Montage == Idle_To_Take_Cover_Montage)
	{
		Debug::Print(TEXT("Take Cover Montage Ended!"));
		
		Start_Take_Cover();
	}

	if(Montage == Exit_Cover_To_Stand)
	{
		Debug::Print(TEXT("Exiting Take Cover Montage"));

		Stop_Take_Cover();
	}
}

FVector UCustom_Movement_Component::Get_Unrotated_Take_Cover_Velocity() const
{
	return UKismetMathLibrary::Quat_UnrotateVector(UpdatedComponent->GetComponentQuat(), Velocity);
}

bool UCustom_Movement_Component::Is_Taking_Cover() const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == E_Custom_Movement_Mode::MOVE_Take_Cover;
}

#pragma endregion


#pragma endregion



#pragma region Parkour_Region


#pragma region Initialize_Parkour

void UCustom_Movement_Component::Attach_Arrow_Actor_To_Character(ATechnical_Animator_Character* Character)
{
	//Use the character pointer passed in at "&UCustom_Movement_Component::Initialize_Parkour_Pointers" by (ATechnical_Animator_Character* Character) to 
	//"GetActorTransform()" and initialize input paramater 1. (for "GetWorld()->SpawnActor") "FTransform Location".
	FTransform Location{Character->GetActorTransform()};
	
	//Initialize Input parameter 2. (for "GetWorld()->SpawnActor") "FActorSpawnParameters Spawn_Info"
	FActorSpawnParameters Spawn_Info{};

	//Spawn the arrow component which is within "&ACharacter_Direction_Arrow" using "GetWorld()->SpawnActor". Use the two input parameters initialized above (Location, Spawn_Info).
	Character_Direction_Arrow = GetWorld()->SpawnActor<ACharacter_Direction_Arrow>(ACharacter_Direction_Arrow::StaticClass(), Location, Spawn_Info);
	
	//After spawning the arrow and attach it to the character using the character pointer passed in by (ATechnical_Animator_Character* Character). Snap it to the target.
	Character_Direction_Arrow->AttachToActor(Character, FAttachmentTransformRules::SnapToTargetIncludingScale);
	
	//Offset the location of the arrow with "SetActorRelativeLocation()" so it is just above the capsule component using its pointer which is initialized above "ACharacter_Direction_Arrow* Character_Direction_Arrow". 
	FVector Character_Direction_Arrow_Relative_Location{FVector(0.f, 0.f, 100.f)};
	Character_Direction_Arrow->SetActorRelativeLocation(Character_Direction_Arrow_Relative_Location);
}

void UCustom_Movement_Component::Get_Pointer_To_Parkour_Locomotion_Interface_Class()
{
	//This cast will return the object that uses the interface. The interface is being used in the Animation Instance, therefore
	//casting "Anim_Instance" to "IParkour_Locomotion_Interface" is nessacary. When Parkour_Interface is used to call the
	//generated function (at compile) "Execute_..." the input argument of the function being called will requre a pointer to the object 
	//which is using the interface. "Anim_Instance" will be passed in as the pointer to this object followed by the other inut parameter(s) 
	//which needs to be filled in.
	Parkour_Interface = Cast<IParkour_Locomotion_Interface>(Anim_Instance);
	if(Parkour_Interface) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Parkour_Interface INITIALIZATION SUCCEEDED"));
	}

	else UE_LOG(LogTemp, Warning, TEXT("Parkour_Interface INITIALIZATION FAILED"));
}

/*void UCustom_Movement_Component::Get_Pointer_To_Parkour_Action_Data_Class()
{
	//Default_Parkour_Data_Asset_Pointer was declared and initialized in the character Blueprint with an empty Data Asset of the type UParkour_Action_Data.
	//This is so an object of the type UParkour_Action_Data* could be created, enabling the calling of functions from within the class UParkour_Action_Data.
	//The Data Asset assigned to the "Default_Parkour_Data_Asset_Pointer" slot within the character Blueprint has no use other use.
	if(Default_Parkour_Data_Asset_Pointer)
	//Parkour_Data_Asset is the pointer which will be used throughout the Parkour System. It holds the address of the class "UParkour_Action_Data" which is set within
	//the constructor of said class followed by the transfer of value which happens within "&UParkour_Action_Data::Get_Pointer_To_This_Class()".
	Parkour_Data_Asset = Default_Parkour_Data_Asset_Pointer->Get_Pointer_To_This_Class();

	if(Parkour_Data_Asset) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Parkour_Data_Asset INITIALIZATION SUCCEEDED"));
	}

	else UE_LOG(LogTemp, Warning, TEXT("Parkour_Data_Asset INITIALIZATION FAILED"));
}*/

void UCustom_Movement_Component::Initialize_Parkour_Pointers(ATechnical_Animator_Character* Character, UMotionWarpingComponent* Motion_Warping, UCameraComponent* Camera)
{
	//"GetCharacterMovement" using the character pointer passed in by (ATechnical_Animator_Character* Character) and initialize "UCharacterMovementComponent* Character_Movement".
	Character_Movement = Character->GetCharacterMovement();
	//Get the mesh using the character pointer passed in by (ATechnical_Animator_Character* Character) and initialize "USkeletalMeshComponent* Mesh".
	Mesh = Character->GetMesh(); 
	//Get the CapsuleComponent by using the character pointer passed in by (ATechnical_Animator_Character* Character) and initialize "UCapsuleComponent* Capsule_Component".
	Capsule_Component = Character->GetCapsuleComponent();
	//Use the "USkeletalMeshComponent* Mesh" which is initialized by the character pointer passed in by (ATechnical_Animator_Character* Character), to get the GetAnimInstance. Initialize "UAnimInstance* Anim_Instance".
	Anim_Instance = Mesh->GetAnimInstance();
	//Initialize "UMotionWarpingComponent* Motion_Warping_Component" with the "UMotionWarpingComponent* Motion_Warping" which is passed in by "&ATechnical_Animator_Character". 
	Motion_Warping_Component = Motion_Warping;
	//Initialize "UCameraComponent* Camera_Component" with the "UCameraComponent* Camera" that is passed in by "&ATechnical_Animator_Character".
	Camera_Component = Camera;
	
	/*Getting a pointer to the Parkour_Locomotion_Interface_Class.*/
	Get_Pointer_To_Parkour_Locomotion_Interface_Class();

	/*Getting a pointer to the Parkour_Action_Data_Class*/
	//Get_Pointer_To_Parkour_Action_Data_Class();
	
	/*Attach Arrow from "&ACharacter_Direction_Arrow" to the Character*/
	Attach_Arrow_Actor_To_Character(Character);
}

#pragma endregion

#pragma region Parkour_Helper

FVector UCustom_Movement_Component::Move_Vector_Up(const FVector& Initial_Location, const float& Move_Value) const
{
	const FVector Move_Direction{UpdatedComponent->GetUpVector()};
	const FVector Destination{Initial_Location + (Move_Direction * Move_Value)};
	
	return Destination;
}

FVector UCustom_Movement_Component::Move_Vector_Down(const FVector& Initial_Location, const float& Move_Value) const
{
	const FVector Move_Direction{UpdatedComponent->GetUpVector()};
	const FVector Destination{Initial_Location + (-Move_Direction * Move_Value)};

	return Destination;
}

FVector UCustom_Movement_Component::Move_Vector_Left(const FVector& Initial_Location, const FRotator& Rotation, const float& Move_Value) const
{
	const FVector Move_Direction{UKismetMathLibrary::GetRightVector(Rotation)};
	const FVector Destination{Initial_Location + (-Move_Direction * Move_Value)};

	return Destination;
}

FVector UCustom_Movement_Component::Move_Vector_Right(const FVector& Initial_Location, const FRotator& Rotation, const float& Move_Value) const
{
	const FVector Move_Direction{UKismetMathLibrary::GetRightVector(Rotation)};
	const FVector Destination{Initial_Location + (Move_Direction * Move_Value)};

	return Destination;
}

FVector UCustom_Movement_Component::Move_Vector_Forward(const FVector& Initial_Location, const FRotator& Rotation, const float& Move_Value) const
{
	const FVector Move_Direction{UKismetMathLibrary::GetForwardVector(Rotation)};
	const FVector Destination{Initial_Location + (Move_Direction * Move_Value)};

	return Destination;
}

FVector UCustom_Movement_Component::Move_Vector_Backward(const FVector& Initial_Location, const FRotator& Rotation, const float& Move_Value) const
{
	const FVector Move_Direction{UKismetMathLibrary::GetForwardVector(Rotation)};
	const FVector Destination{Initial_Location + (-Move_Direction * Move_Value)};

	return Destination;
}

FRotator UCustom_Movement_Component::Add_Rotator(const FRotator& Initial_Rotation, const float& Add_To_Rotation) const
{
	const FRotator New_Rotation_Roll{Initial_Rotation.Roll + Add_To_Rotation};
	const FRotator New_Rotation_Pitch{Initial_Rotation.Pitch + Add_To_Rotation};
	const FRotator New_Rotation_Yaw{Initial_Rotation.Yaw + Add_To_Rotation};
	const FRotator New_Rotation{New_Rotation_Roll + New_Rotation_Pitch + New_Rotation_Yaw};

	return New_Rotation;
}

FRotator UCustom_Movement_Component::Reverse_Wall_Normal_Rotation_Z(const FVector& Initial_Wall_Normal) const
{
	                                              //The direction of the wall normal is its X axis (pointing forward).
	const FRotator Wall_Normal{UKismetMathLibrary::MakeRotFromX(Initial_Wall_Normal)};
	const FRotator Reverse_Wall_Rotation_On_Z_Axis_180{Wall_Normal - FRotator(0, 180, 0)};
	const FRotator Delta_Rotaton{Reverse_Wall_Rotation_On_Z_Axis_180};
	
	return  Delta_Rotaton;
}

void UCustom_Movement_Component::Draw_Debug_Sphere(const FVector& Location, const float& Radius, const FColor& Color, const float& Duration, const bool& bDraw_Debug_Shape_Persistent, const float& Lifetime) const
{
	UWorld* World = GetWorld();

	DrawDebugSphere(
		World,
		Location,
		Radius,
		12,
		Color,
		bDraw_Debug_Shape_Persistent,
		Lifetime
	);
}

#pragma endregion

#pragma region Parkour_Traces

void UCustom_Movement_Component::Parkour_Detect_Wall()
{	
	//This function will return this FHitResult. ALso, the "SphereTraceSingleForObjects" performed in the for loop will fill this FHitResult with data.
	FHitResult Out_Hit{};
	//Initialize the last index to be used in the for loop.
	int For_Loop_Last_Index{};
	//Depending on the status of the character one of these integers will be used to set the last index for the for loop.
	const int Is_Falling{7};
	const int Is_Not_Falling{14};
	
	//Set the last index for the for loop.
	//If the character is falling there should be less ray casts performed to decrease the "height" of all the sphere traces combined together.
	if(Character_Movement)
	{
		if(Character_Movement->IsFalling())
		For_Loop_Last_Index = Is_Falling;

		else //if(!Character_Movement->IsFalling())
		For_Loop_Last_Index = Is_Not_Falling;
	}


	for(int Index{}; Index < For_Loop_Last_Index; Index++)
	{
		//Get the location of the character.
		const FVector Component_Location{UpdatedComponent->GetComponentLocation()};
		//Set the vector which the sphere trace will use to be at ground level. This for loop will create new sphere traces from this location. Each sphere trace will stack ontop of the previous forming a "tower".
		const FVector Set_Vector_At_Ground_Level{Move_Vector_Down(Component_Location, 70.f)};
		//With each iteration of the for loop move the vector up 17 units by multiplying the index by 17.
		const FVector Move_Vector_Up_With_Each_Iteration_Of_Loop{Move_Vector_Up(Set_Vector_At_Ground_Level, Index * 17.f)};
		//Move the vector backwards 20 units so that it starts right behind the character.
		const FVector Start{Move_Vector_Backward(Move_Vector_Up_With_Each_Iteration_Of_Loop, UpdatedComponent->GetComponentRotation(), 20)};
		//Move the vector forward 140 units so that it ends a good distance away from the character.
		const FVector End{Move_Vector_Forward(Start, UpdatedComponent->GetComponentRotation(), 140)};

		//Develop a "SphereTraceSingleForObjects()". The objects will be set in the character blueprint.
		UKismetSystemLibrary::SphereTraceSingleForObjects(
			this,
			Start,
			End,
			10.f,
			Parkour_Detect_Wall_Trace_Types,
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::None,
			Out_Hit,
			false
			);

		//If there is a blocking hit and there is no initial overlap break out of the for loop early.
		if(Out_Hit.bBlockingHit && !Out_Hit.bStartPenetrating)
		{
			Initial_Ground_Level_Front_Wall_Hit_Result = Out_Hit;
			break;
		}
		
	}
	//Drawing debug sphere so the "EDrawDebugTrace" can be set to none on the "SphereTraceSingleForObjects()".
	//Draw_Debug_Sphere(Initial_Ground_Level_Front_Wall_Hit_Result.ImpactPoint, 5.f, FColor::Blue, 1.f, false, 7.f);
} 

void UCustom_Movement_Component::Grid_Scan_For_Hit_Results(const FVector& Previous_Trace_Location, const FRotator& Previous_Trace_Rotation, const int& Scan_Width_Value, const int& Scan_Height_Value)
{
	/*The input parameter "Previous_Trace_Rotation" is the Impact Normal for the "SphereTraceSingleForObjects()" found in "&UCustom_Movement_Component::Parkour_Detect_Wall()". 
	This normal will be reversed when this function is called so that the ray cast performed in this function will face towards the wall of the impact point passed into "Previous_Trace_Location".
	This impact point is also foundin "&UCustom_Movement_Component::Parkour_Detect_Wall() "Scan_Width_Value" and "Scan_Height_Value" will are variables which may be set in C++ or in the character Blueprint*/
	
	
	//The "LineTraceSingleForObjects()" performed in the for loop will fill this FHitResult with data.
	FHitResult Out_Hit{};

	//In this case it's good practice to empty the arrays at the beginnning of the function so that garagbage data from the previous function call can be wiped out.
	Grid_Scan_Hit_Traces.Empty();
	
	//Develop a for loop which will set the "Width" of the line traces performed. The goal is to have the line traces casted form a grid which have has a width of 5 line traces.
	//"Scan_Width_Value" will be filled with the the value 5.
	for(int Index_1{}; Index_1 <= Scan_Width_Value; Index_1++)
	{	
		//Multiply "Index_1" by 20 on each iteration of the for loop. This value will be subtracted from to generate the vector location of each of the line traces which make up the width of "grid"
		const int Index_1_Multiplier{Index_1 * 20};
		
		//Multiply the "Scan_Width_Value" by 10 during each iteration of the for loop. This value will be subtracted from "Index_1_Multiplier" during each iteration of the loop to position the vector 
		//which will serve as the location for respective the ray cast. 
		const int Scan_Width_Value_Multiplier{Scan_Width_Value * 10};
		
		//Subtract the value in "Scan_Width_Value_Multiplier" from the value in "Index_1_Multiplier" to position the vector accordingly.
		const int Set_Scan_Width_Trace_Location_Value{Index_1_Multiplier - Scan_Width_Value_Multiplier};
		
		//With the calculation (subtraction) above, on the first iteration of this for loop, the vector will start off at the left of the input parameter "Previous_Trace_Location" which is 
		//the impact point of the "SphereTraceSingleForObjects()" found in "&UCustom_Movement_Component::Parkour_Detect_Wall()". This is because the value will start of as a negative number 
		//on loop 0 and in result instead of "Move_Vector_Right" the vector will move to the left (because of the negative value). WIth each loop iteration, the vector will move to the right 
		//as the value increases. When "Index_1" is 2 the value of "Set_Scan_Width_Trace_Location_Value" will be 0 and in result the vector will be at the same location as the input parameter 
		//"Previous_Trace_Location". This will be followed by two more loops which will move the vector to the right of the "SphereTraceSingleForObjects()" found in "&UCustom_Movement_Component::Parkour_Detect_Wall()"  
		//(input parameter "Previous_Trace_Location"). In result there will be five raycast formed, hence forming the "width" of the grid scan.
		const FVector Set_Scan_Width_Trace_Location{Move_Vector_Right(Previous_Trace_Location, Previous_Trace_Rotation, Set_Scan_Width_Trace_Location_Value)};

		//Develop a nested for loop to handle the height of the grid scan.
		//"Scan_Height_Value" will be filled with the the value 30.
		for(int Index_2{}; Index_2 <= Scan_Height_Value; Index_2++)
		{
			//Multiply "Index_2" by 8 on each iteration of the for loop. This value will be used to generate the vector location of each of the line traces which make up the height of "grid" 
			const int Index_2_Multiplier{Index_2 * 8};
			
			//With each iteration of this nested loop the vector will have a starting point of the "Set_Scan_Width_Trace_Location" which happens to be at ground level. Therefore, to increase the height of the vector
			//consistently, making sure the position of the vector is above the position of the previous vector a multiplication of (Index_2 * 8) found in "Index_2_Multiplier" has to be done during each loop. 
			const FVector Set_Scan_Height_Trace_Location{Move_Vector_Up(Set_Scan_Width_Trace_Location, Index_2_Multiplier)};
			
			//Move the vector backwards so that it starts right in front of the character.
			const FVector Start{Move_Vector_Backward(Set_Scan_Height_Trace_Location, Previous_Trace_Rotation, 70.f)};
			
			//Move the vector forwards so that the line traces end a good distance away from the character
			const FVector End{Move_Vector_Forward(Start, Previous_Trace_Rotation, 80.f)};

			//Develop the line traces
			UKismetSystemLibrary::LineTraceSingleForObjects(
				this,
				Start,
				End,
				Parkour_Grid_Scan_For_Hit_Results_Trace_Types,
				false,
				TArray<AActor*>(),
				EDrawDebugTrace::ForOneFrame,
				Out_Hit,
				true
			);

			//Add the "Out_Hit" generated by each for loop iteration to the array "Wall_Hit_Traces_Height".
			Grid_Scan_Hit_Traces.Add(Out_Hit);
		}
	}
}

void UCustom_Movement_Component::Analyze_Grid_Scan_For_Front_Wall_Top_Edge_Hits()
{
	//In this case it's good practice to empty the arrays at the beginnning of the function so that garagbage data from the previous function call can be wiped out.
	Front_Wall_Top_Edge_Traces.Empty();

	//Before moving on to perform the following for loop the loop index has to be greater than 0 before anything can be done. This is because there is a need for at least two indexes (current and previous)
	//to perform the calculations needed. The goal of the following for loop is to get the distances (from Trace Start to Trace End if there is no impact point and from Trace Start to Impact Point if there is a blocking hit) 
	//of the line traces which were generated in the previous for loop. Once the distances of line traces are calculated for both the current (Index_3) and previous (Index_3 - 1) loop iteration, there is a check to see wheahter there is a blocking hit or not. 
	//Depending on this answer the corresponding value will be assighned to the global variables "Distance_In_Grid_Scan_For_Hit_Results_Current_Iteration" and "Distance_In_Grid_Scan_For_Hit_Results_Previous_Iteration".
	//After this there is another check to see if the difference between the two is greater than 7 units. If this is true this means the previous line trace is the ray cast that is is right under the top edge of the wall.
	//In result, said line trace from the previous loop iteration is added to the TArray "Front_Wall_Top_Edge_Traces" and the for loop "continues".
		
	for(int Index{}; Index != Grid_Scan_Hit_Traces.Num(); Index++)
	{
		/*The reason why this "continue" happens at the beginning of this array index based for loop (which handles the calculations for the current and previous index's line trace) is because there needs to be at least two array elements (Index = 1)
		loaded into this for loop for it to work correctly.*/
		if(Index == 0) continue;

		/*Getting the trace distance from the current loop iteration*/
		//Checking to see if the current line trace has a blocking hit or not. Depending on this answer the appropriate 
		//distance value will be assigned to the global variable "Distance_In_Grid_Scan_For_Hit_Results_Current_Iteration".
			
		//If there is a blocking hit this, is the distance from the line trace start to the impact point.
		const float Current_Iteration_Distance_If_Blocking_Hit{Grid_Scan_Hit_Traces[Index].Distance};
				
		//If there is no impact point get the distance between the line trace start and its end.
		const FVector_NetQuantize Current_Iteration_Line_Trace_Start{Grid_Scan_Hit_Traces[Index].TraceStart};
		const FVector_NetQuantize Current_Iteration_Line_Trace_End{Grid_Scan_Hit_Traces[Index].TraceEnd};
		const double Current_Iteration_Distance_If_No_Blocking_Hit{UKismetMathLibrary::Vector_Distance(Current_Iteration_Line_Trace_Start, Current_Iteration_Line_Trace_End)};

		//Depending on whether  there is a impact point, assign the corresponding value to "Distance_In_Grid_Scan_For_Hit_Results_Current_Iteration".
		if(Grid_Scan_Hit_Traces[Index].bBlockingHit) Distance_In_Grid_Scan_For_Hit_Results_Current_Iteration = Current_Iteration_Distance_If_Blocking_Hit;
		else if(!Grid_Scan_Hit_Traces[Index].bBlockingHit) Distance_In_Grid_Scan_For_Hit_Results_Current_Iteration = Current_Iteration_Distance_If_No_Blocking_Hit;


		/*Getting the trace distance in the previous loop iteration*/
		//Checking to see if the previous line trace has a blocking or not. Depending on this answer the appropriate 
		//distance value will be assigned to the global variable "Distance_In_Grid_Scan_For_Hit_Results_Previous_Iteration".
		int Previous_Index{Index -1};

		//Getting a reference to the previous element (in respect to the current iteration of this nested for loop) in the TArray "Grid_Line_Hit_Trace".
		FHitResult& Previous_Iteration_Line_Trace_Reference{Grid_Scan_Hit_Traces[Previous_Index]};

		/*Getting the trace distance in previous loop iteration*/
		//Checking to see if the previous line trace has a blocking hit or not. Depending on this answer the appropriate 
		//distance value will be assigned to the global variable "Distance_In_Grid_Scan_For_Hit_Results_Previous_Iteration".
			
		//If there is a blocking hit this, is the distance from the line trace start to the impact point.
		const float Previous_Iteration_Distance_If_Blocking_Hit{Previous_Iteration_Line_Trace_Reference.Distance};

		//If there is no impact point get the distance between the line trace start and its end.
		const FVector_NetQuantize Previous_Iteration_Line_Trace_Start{Previous_Iteration_Line_Trace_Reference.TraceStart};
		const FVector_NetQuantize Previous_Iteration_Line_Trace_End{Previous_Iteration_Line_Trace_Reference.TraceEnd};
		const double Previous_Iteration_Distance_If_No_Blocking_Hit{UKismetMathLibrary::Vector_Distance(Previous_Iteration_Line_Trace_Start, Previous_Iteration_Line_Trace_End)};

		//Depending on whether  there is a impact point, assign the corresponding value to "Distance_In_Grid_Scan_For_Hit_Results_Previous_Iteration".
		if(Previous_Iteration_Line_Trace_Reference.bBlockingHit) Distance_In_Grid_Scan_For_Hit_Results_Previous_Iteration = Previous_Iteration_Distance_If_Blocking_Hit;
		else if(!Previous_Iteration_Line_Trace_Reference.bBlockingHit) Distance_In_Grid_Scan_For_Hit_Results_Previous_Iteration = Previous_Iteration_Distance_If_No_Blocking_Hit;
			
		//Get the difference between the assigned distances (whether there is a blocking hit or not) of current and the previous line traces.
		const double Distance_Between_Current_And_Previous_Line_Trace{Distance_In_Grid_Scan_For_Hit_Results_Current_Iteration - Distance_In_Grid_Scan_For_Hit_Results_Previous_Iteration};

		//If the difference between the assigned distances of current and the previous line traces is greater than 7 units this means the previous line trace is the line is right under the top edge of the wall.
		//In result, said line trace from the previous loop iteration is added to the TArray "Front_Wall_Top_Edge_Traces" and the nested for loop "continues"
		if(Distance_Between_Current_And_Previous_Line_Trace > 7)
		{
			Front_Wall_Top_Edge_Traces.Add(Previous_Iteration_Line_Trace_Reference);
			Debug::Print(FString(TEXT("Differences in Front_Wall_Top_Edge_Traces: ") + FString::FromInt(Distance_Between_Current_And_Previous_Line_Trace)), FColor::Blue, 5);
			//Draw_Debug_Sphere(Previous_Iteration_Line_Trace_Reference.ImpactPoint, 5.f, FColor::Black, 10.f, false, 1);
			continue;
		}
		else continue;
	}
}

void UCustom_Movement_Component::Realize_Front_Wall_Top_Edge_Best_Hit()
{
	//During the first iteration of the loop (Index == 0), assign the first element of the array in "Front_Wall_Top_Edge_Traces" into the global variable "Front_Wall_Top_Edge_Best_Hit".
	//This will only happen one time during this for loop. The global FHitResult variable "Front_Wall_Top_Edge_Best_Hit" needs to have a FHitResult assigned to it so said FHitResult
	//can be compared to the other FHitResults in said array. During each for loop iteration, the FHitResult found in the current element of the array (the current Index) will be compared 
	//to the FHitResult which was assigned to the global FHitResult variable "Front_Wall_Top_Edge_Best_Hit" (during Index == 0). When compared to the current location of the character, 
	//if the FHitResult in the element of the array (the current Index) has a smaller delta than that of the FHitResult stored in the global FHitResult variable 
	//"Front_Wall_Top_Edge_Best_Hit" then the FHitResult in the element of the array (the current Index) will replace the current FHitResult stored in the global FHitResult variable
	//"Front_Wall_Top_Edge_Best_Hit". At the end of the for loop the FHitResult which has the lowest delta when compared to the current location of the character will be stored in the 
	//global FHitResult variable "Front_Wall_Top_Edge_Best_Hit".

	const FVector Current_Copmponent_Location{UpdatedComponent->GetComponentLocation()};
	
	for(int Index{}; Index != Front_Wall_Top_Edge_Traces.Num(); Index++)
	{
		//Initialize the global FHitResult variable with the first element of the array. This will only happen once.
		if(Index == 0) Front_Wall_Top_Edge_Best_Hit = Front_Wall_Top_Edge_Traces[Index];
		
		else
		{	
			//Obtain the locatation of the impact points for the FHitResult stored in the global variable "Front_Wall_Top_Edge_Best_Hit" and the FHitResult which is
			//at the same element of that as the current loop iteration (Index).
			const FVector Current_Front_Wall_Top_Edge_Best_Hit_Location{Front_Wall_Top_Edge_Best_Hit.ImpactPoint};
			const FVector Current_Iteration_Trace_Location{Front_Wall_Top_Edge_Traces[Index].ImpactPoint};
			
			//Obtain the delta of the impact points for the FHitResult stored in the global variable "Front_Wall_Top_Edge_Best_Hit" and the FHitResult which is
			//at the same element of that as the current loop iteration (Index), when compared to the current location of the character.
			const double Delta_Between_Current_Iteration_Trace_Location_And_Component_Location
			{UKismetMathLibrary::Vector_Distance(Current_Iteration_Trace_Location, Current_Copmponent_Location)};
			
			const double Delta_Between_Current_Front_Wall_Top_Edge_Best_Hit_And_Component_Location
			{UKismetMathLibrary::Vector_Distance(Current_Front_Wall_Top_Edge_Best_Hit_Location, Current_Copmponent_Location)};

			//If the FHitResult in the element of the array (the current Index) has a smaller delta than that of the FHitResult stored in the global FHitResult variable 
			//"Front_Wall_Top_Edge_Best_Hit" when compared to the current location of the character, then the FHitResult in the element of the array (the current Index) 
			//will replace the current FHitResult stored in the global FHitResult variable "Front_Wall_Top_Edge_Best_Hit"
			if(Delta_Between_Current_Iteration_Trace_Location_And_Component_Location <= Delta_Between_Current_Front_Wall_Top_Edge_Best_Hit_And_Component_Location)
			Front_Wall_Top_Edge_Best_Hit = Front_Wall_Top_Edge_Traces[Index];

			//If the FHitResult in the element of the array (the current Index) does not have a smaller delta than that of the FHitResult stored in the global FHitResult variable 
			//"Front_Wall_Top_Edge_Best_Hit" when compared to the current location of the character, then no change happens regarding which FHitResult is stored in the global 
			//FHitResult variable "Front_Wall_Top_Edge_Best_Hit" and the for loop "continues"
			else continue;
		}	
	}

	//Draw_Debug_Sphere(Front_Wall_Top_Edge_Best_Hit.ImpactPoint, 10.f, FColor::Cyan, 5.f, false, 5.f);
}

void UCustom_Movement_Component::Analyze_Wall_Top_Surface()
{
	//Check to see if the Parkour State is set to climb. If this is the case call the function "Calculate_Wall_Top_Surface()".
	//Said function will generate the traces for the top of the wall and the depth of the wall.

	//If the Parkour State is not set to climb then the character is not climbing and is either falling or on the ground. Therefore, before 
	//calling the function "Calculate_Wall_Top_Surface()" the front wall which is being analyzed must have "its normal reversed on the Z axis" 
	//via the FHitResult "Front_Wall_Top_Edge_Best_Hit" using the helper function created "Reverse_Wall_Normal_Rotation_Z()". 
	//This is how the character will know which direction to perform the parkour action.
	
	if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))))
	{
		Calculate_Wall_Top_Surface();
	}
	
	else if(Parkour_State != FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))))
	{
		Reversed_Front_Wall_Normal_Z = Reverse_Wall_Normal_Rotation_Z(Front_Wall_Top_Edge_Best_Hit.ImpactNormal);
		Calculate_Wall_Top_Surface();
	}
}

void UCustom_Movement_Component::Calculate_Wall_Top_Surface()
{
	FHitResult Out_Hit_1{};
	FHitResult Out_Hit_2{};
	
	FHitResult Top_Wall_Last_Hit{};

	//This for loop throws sphere traces downwards, starting from an elevated offset location which originates from the global FHitResult "Front_Wall_Top_Edge_Best_Hit".
	//the starting location of each sphare trace will be moved forward 30 units from the previous sphere trace. There will be a maximum of nine sphere traces generated.
	for(int Index{}; Index <= 8; Index++)
	{
		//Multiplier used to move each sphere trace forward 30 units from the sphere trace which was generated during the previous for loop iteration (considering the Index is not 0).
		const int Index_Multiplier{Index * 30};
		
		//Move the vector forward from its staring location which is the FHitResult "Front_Wall_Top_Edge_Best_Hit" impact point using the value in "Index_Multiplier". To make the vector move on the charater's forward direction
		//"the normal of the wall must be reversed". To do this the global FRotator variable "Reversed_Front_Wall_Normal_Z" is used. This variable is filled with the current wall that is being 
		//analyzed reversed normal on the Z axis. This happens in the function "&UCustom_Movement_Component::Analyze_Wall_Top_Surface()".
		const FVector Move_Vector_Forward_With_Each_Iteration_Of_Loop{Move_Vector_Forward(
			Front_Wall_Top_Edge_Best_Hit.ImpactPoint, 
			Reversed_Front_Wall_Normal_Z, 
			Index_Multiplier)};
		
		const FVector Start{Move_Vector_Up(Move_Vector_Forward_With_Each_Iteration_Of_Loop, 7.f)};
		const FVector End{Move_Vector_Down(Start, 7.f)};

		UKismetSystemLibrary::SphereTraceSingleForObjects(
			this,
			Start,
			End,
			5.f,
			Parkour_Analyzing_Top_Wall_Surface_Trace_Types,
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::None,
			Out_Hit_1,
			false
			);
		
		//If there is no blocing hit on any of the sphere traces break out of the for loop. This is because the top surface of the wall which is being analyzed had dropped below the threshold which is desired (Sphere trace "End").
		if(!Out_Hit_1.bBlockingHit)
		break;

		//If the index is 0 and there is a blocking hit on the Out_Hit of the current loop iteration (Index = 0), assign said FHitResult to the global FHitResult variable "Wall_Top_Result". This is the first and closest FHitResult
		//to the charater on the top surface of the wall which is being analyzed.
		else if(Index == 0 && Out_Hit_1.bBlockingHit)
		{
			Wall_Top_Result = Out_Hit_1;
			//Draw_Debug_Sphere(Wall_Top_Result.ImpactPoint, 15.f, FColor::Emerald, 7.f, false, 7.f);
		}
		
		//If the current loop iteration is not 0 then assign every other hit result to the local FHitResult variable "Top_Wall_Last_Hit". With each for loop iteration the data in this variable will be overitten and in result by the end of the 
		//for loop the last hit will be stored in said local variable. The FHitResult stored in this local variable will be used to calculate the global FHitResult "Wall_Depth_Result".
		else if(Index != 0 && Out_Hit_1.bBlockingHit)
		{
			Top_Wall_Last_Hit = Out_Hit_1;
			//Draw_Debug_Sphere(Top_Wall_Last_Hit.ImpactPoint, 10.f, FColor::Magenta, 7.f, false, 7.f);
		}
	}

	//If the "Parkour_State" is set to "Parkour.State.Free.Roam" then the character is on the ground. Therefore, a calculation must be made to determine what data to store in the global
	//FHitResult variable "Wall_Depth_Result". The data stored in said global variable will be used to determine the data to store in the other global varable "Wall_Vault_Result".
	//This only needs to be done when the "Parkour_State" is set to "Parkour.State.Free.Roam" because the FHitResults stored in the two global variables "Wall_Depth_Result" and "Wall_Vault_Result"
	//are only useful for determining which vault action to perform. Vault actions can only be performed when the "Parkour_State" is set to "Parkour.State.Free.Roam" (the character is on the ground).
	if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Free.Roam"))))
	{
		//The goal with this sphere trace is to execute a ray cast which starts at an offset position from the FHitResult "Top_Wall_Last_Hit". This offset should be alligned with the normal stored in the global variable
		//"Reversed_Front_Wall_Normal_Z". So visually, it should start in the same "trajectory" which the sphere traces in the previous for loop were being generated in. This sphere trace should end where the local FHitResult 
		//"Top_Wall_Last_Hit" impact point is. With a radius just big enough, it should collide with the wall which is being analyzed, on the opposite side from where the character is (this sphere trace should be launched towards 
		//the character from the other side of the wall being analyzed).
		const FVector Start{Move_Vector_Forward(Top_Wall_Last_Hit.ImpactPoint, Reversed_Front_Wall_Normal_Z, 30.f)};
		const FVector End{Top_Wall_Last_Hit.ImpactPoint};

		UKismetSystemLibrary::SphereTraceSingleForObjects(
			this,
			Start,
			End,
			10.f,
			Parkour_Wall_Depth_Trace_Types,
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::None,
			Out_Hit_2,
			false
		);

		//Store the sphere trace performed in the global FHitResult variable "Wall_Depth_Result"
		Wall_Depth_Result = Out_Hit_2;
		//Draw_Debug_Sphere(Wall_Depth_Result.ImpactPoint, 10.f, FColor::Purple, 10.f, false, 10.f);
	}
}

void UCustom_Movement_Component::Calculate_Wall_Vault_Location()
{
	//This function calculates the vault location in which the character will land on.
	//The starting position of this sphere trace is an offset from the impact point of the FHitResult 
	//which is stored in the global variable "Wall_Depth_Result". The normal stored in "Reversed_Front_Wall_Normal_Z"
	//is used to set the vector normal be the same as that of the front wall's reversed normal on the Z axis.
	//This vector is moved forward 70 units (away from the character since its normal on the z axis is reversed using 
	//"Reversed_Front_Wall_Normal_Z". This offset is the trace start. From this location the vector is sent downwards 200 units (to make sure it hit a surface).
	//This location is the trace end location.
	
	FHitResult Out_Hit{};

	const FVector Start{Move_Vector_Forward(Wall_Depth_Result.ImpactPoint, Reversed_Front_Wall_Normal_Z, 70.f)};
	const FVector End{Move_Vector_Down(Start, 200.f)};
	
	UKismetSystemLibrary::SphereTraceSingleForObjects(
		this,
		Start,
		End,
		10.f,
		Parkour_Vault_Trace_Types,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		Out_Hit,
		false
	);

	//Store the FHitResult generated by the sphere trace into the global FHitResult variable "Wall_Vault_Result".
	Wall_Vault_Result = Out_Hit;

	//Draw_Debug_Sphere(Wall_Vault_Result.ImpactPoint, 10.f, FColor::Silver, 10.f, false, 10.f);
}

void UCustom_Movement_Component::Calculate_Wall_Height()
{
	if(Wall_Top_Result.bBlockingHit)
	{
		const FVector Character_Root_Bone_Location{Mesh->GetSocketLocation(FName(TEXT("root")))};
		const FVector Wall_Top_Result_Trace_Location{Wall_Top_Result.ImpactPoint};
		const double  Height_Delta_Between_Character_Root_Bone_Location_And_Wall_Top_Result_Trace_Location
		{Wall_Top_Result_Trace_Location.Z - Character_Root_Bone_Location.Z};
		
		Wall_Height = Height_Delta_Between_Character_Root_Bone_Location_And_Wall_Top_Result_Trace_Location;
		Debug::Print(FString("Wall_Height: ") + FString::SanitizeFloat(Wall_Height), FColor::MakeRandomColor(), 3);
	}

	else
	{
		Wall_Height = 0.f;
		Debug::Print(FString("Wall_Height: ") + FString::SanitizeFloat(Wall_Height), FColor::MakeRandomColor(), 3);
	}
}

void UCustom_Movement_Component::Calculate_Wall_Depth()
{
	if(Wall_Depth_Result.bBlockingHit)
	{
		const FVector Wall_Depth_Result_Trace_Location{Wall_Depth_Result.ImpactPoint};
		const FVector Wall_Top_Result_Trace_Location{Wall_Top_Result.ImpactPoint};
		const double  Distance_Delta_Between_Wall_Depth_Result_Trace_Location_And_Wall_Top_Result_Trace_Location
		{UKismetMathLibrary::Vector_Distance(Wall_Depth_Result_Trace_Location, Wall_Top_Result_Trace_Location)};

		Wall_Depth = Distance_Delta_Between_Wall_Depth_Result_Trace_Location_And_Wall_Top_Result_Trace_Location;
		Debug::Print(FString("Wall_Depth: ") + FString::SanitizeFloat(Wall_Depth), FColor::MakeRandomColor(), 9);
	}
		
	else
	{
		Wall_Depth = 0;
		Debug::Print(FString("Wall_Depth: ") + FString::SanitizeFloat(Wall_Depth), FColor::MakeRandomColor(), 9);
	} 
}

void UCustom_Movement_Component::Calculate_Vault_Height()
{
	if(Wall_Vault_Result.bBlockingHit)
	{
		const FVector Wall_Vault_Result_Trace_Location{Wall_Vault_Result.ImpactPoint};
		const FVector Wall_Depth_Result_Trace_Location{Wall_Depth_Result.ImpactPoint};
		const double  Height_Delta_Between_Wall_Depth_Result_Trace_Location_And_Wall_Vault_Result_Trace_Location
		{Wall_Depth_Result_Trace_Location.Z - Wall_Vault_Result_Trace_Location.Z};

		Vault_Height = Height_Delta_Between_Wall_Depth_Result_Trace_Location_And_Wall_Vault_Result_Trace_Location;
		Debug::Print(FString("Vault_Height: ") + FString::SanitizeFloat(Vault_Height), FColor::MakeRandomColor(), 4);
	}
		
	else
	{
		Vault_Height = 0;
		Debug::Print(FString("Vault_Height: ") + FString::SanitizeFloat(Vault_Height), FColor::MakeRandomColor(), 4);
	} 
}

void UCustom_Movement_Component::Validate_bIs_On_Ground()
{
	/*This function will be called every "Tick()" within the function "Parkour_Call_In_Tick()". The goal of this function is to determine whether the character is 
	grounded or airborne. By using a BoxTraceSingleForObjects that generates a box ray cast that is located at the same location as the root bone of the character 
	the result of whether there is a blocking hit or not stored in the local FHitResult variable "Out_Hit" can be used to set the value of the global bool variable 
	"bIs_On_Ground".*/

	//If the current "Parkour_State" set on the character is "Parkour.State.Climb" then it is evident that the character is not grounded. In this case the global bool
	//variable "bIs_On_Ground" will be set to false.
	if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))))
	{
		bIs_On_Ground = false;
	}

	else
	{
		FHitResult Out_Hit{};

		UKismetSystemLibrary::BoxTraceSingleForObjects(
			this,
			Mesh->GetSocketLocation(FName(TEXT("root"))),
			Mesh->GetSocketLocation(FName(TEXT("root"))),
			FVector(10, 10, 4),
			UKismetMathLibrary::MakeRotFromX(UpdatedComponent->GetForwardVector()),
			Parkour_Validate_On_Land_Trace_Types,
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::ForOneFrame,
			Out_Hit,
			false
		);

		if(Out_Hit.bBlockingHit)
		bIs_On_Ground = true;

		else
		bIs_On_Ground = false;
	}
}

void UCustom_Movement_Component::Decide_Climb_Style(const FVector& Impact_Point, const FRotator& Direction_For_Character_To_Face)
{
	/*The goal of this function is to use the impact point of the global FHitResult "Wall_Top_Result" and the direction of 
	the global FRotator "Reversed_Front_Wall_Normal_Z as the cornerstone of a new sphere trace. The location filled into the 
	input parameter "Impact_Point" is that of the global FHitResult "Wall_Top_Result" and the Rotation filled into the input
	argument "Direction_For_Character_To_Face" is that of the global FRotator "Reversed_Front_Wall_Normal_Z. 
	Using the helper function "Move_Vector_Down()", this location will be moved down 125 units. Next, from that location 
	it will be moved backwards 10 units using the helper function "Move_Vector_Backwards (this is the start location of 
	the sphere trace). Finally from the start location of the sphere trace the vector will be moved forward 25 units 
	(this will be the end location of the sphere trace). If the local FHitResult "Out_Hit" has a blocking hit then this 
	means there is a wall in front of the character (at the height of where the feet will be during a Braced_Climb) and 
	the climb style should be braced. If there is no blocking hit then this means there is no wall in front of the character 
	(at the height of where the feet will be during a Braced_Climb) and the climb style should be Free_Hang. The idea of this 
	sphere trace is to cast a ray trace from the height of where the character's feet will be during a braced climb to 
	determine if there is a wall there for the feet to land on during a Braced_Climb.*/
	FHitResult Out_Hit{};
	const FVector Move_Vector_Down_To_Feet_Level{Move_Vector_Down(Impact_Point, 125)};
	const FVector Start{Move_Vector_Backward(Move_Vector_Down_To_Feet_Level, Direction_For_Character_To_Face, 10.f)};
	const FVector End{Move_Vector_Forward(Start, Direction_For_Character_To_Face, 25.f)};

	UKismetSystemLibrary::SphereTraceSingleForObjects(
		this,
		Start,
		End,
		10.f,
		Parkour_Decide_Climb_Style_Trace_Types,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::ForOneFrame,
		Out_Hit,
		false
		);
	
	if(Out_Hit.bBlockingHit)
	{
		Set_Parkour_Climb_Style(FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Climb.Style.Braced.Climb"))));
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(48.f);
	}
	

	else if(!Out_Hit.bBlockingHit)
	{
		Set_Parkour_Climb_Style(FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Climb.Style.FreeHang"))));
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(98.f);
	}
}

bool UCustom_Movement_Component::Parkour_Climb_State_Detect_Wall(FHitResult& Parkour_Climbing_Detect_Wall_Hit_Result, FHitResult& Parkour_Climbing_Wall_Top_Result)
{
	//The main goal of this function is to detect whether or not there is a wall infront of the character when the global FGameplayTag "Parkour_State" is set to 
	//"Parkour.State.Climb". When the character is shimmying across the surface of a wall the mountable surface of the wall may end and therefore the charcater will need to
	//stop moving. There are two sphere traces which happen in this function. The sphere trace in the outer for loop handles determining whether or not there is a wall 
	//ahead of the character. The sphere trace in the inner for loop uses the impact point of the sphere trace of the outer for loop to determine the wall top result while
	//shimmying across the wall. This sphere trace also handles determining whether or not there is another enough space above the surface of the hands to continue shimmying. 

	//Since the value stored in the global double variable "Right_Left_Movement_Value" has a maximum of 1 (value represents the input from the controller for whether the character should move to the 
	//left or right with the value 1 being full input in the respective direction and 0 being no input for the character to move), whatever value is stored in said global variable will be multiplied
	//by 10 within the local double variable "Horizontal_Move_Direction_Update". 
	const float Right_Left_Movement_Value_Multiplier{10.f};

	//Get the value which the controller is putting into the global double value "Right_Left_Movement_Value" and multiply it by 10. This product is used as an offset value to begin generating the ray 
	//casts in the outer for loop. If the value is a negative number the ray cast will be on the left side of the arrow actor (which means the character is moving to the left). If the value is not
	//a negative number then the character is moving to the right and the sphere traces will be on the right side of the arrow actor. 
	const double Horizontal_Move_Direction_Update{Right_Left_Movement_Value * Right_Left_Movement_Value_Multiplier};

	//Offset the start location of the sphere trace according to the current FGameplayTag set on the global FGameplayTag variable "Parkour_Climb_Style".
	//This needs to happen because during shimmying across some surfaces the character moves to close to the surface of the wall. If this happens then the sphere trace
	//will not get a blocking hit abnd in result no wall will be detected.
	const float Value_To_Offset_Sphere_Trace_Backwards{Select_Value_Based_On_Climb_Style(Parkour_Climb_Style, 10.f, 30.f)};

	int Index_1{};
	//This sphere trace generates sphere traces to the right or left of the arrow actor which is placed right ontop of the character. The side of the arrow actor which
	//the sphere traces are generated is determined by the value placed into the global double variable "Right_Left_Movement_Value". The value here is calculated by the input
	//into the controller within the function "&UCustom_Movement_Component::Add_Movement_Input" Three sphere traces are generated downwards until there is a blocking hit. If no
	//all sphere traces are generated and there is no blocking hit, then this means there is no surface which the character can shimmy across. If ".bStartPenetrating" is true
	//during any iteration of this for loop then this means the character is too close to the surface. In both instances "Stop_Parkour_Climb_Movement_Immediately_And_Reset_Movement_Input_Variables", 
    //"Reset_Movement_Input_Variables" and "return" should be called.
	for(Index_1; Index_1 <= 7; Index_1++)
	{
		//Get locattion of the Arrow actor.
		const FVector Arrow_Location{Character_Direction_Arrow->GetActorLocation()};
		//Get the rotation of the Arrow actor
		const FRotator Arrow_Direction{Character_Direction_Arrow->GetActorRotation()};
		
		//Offset the location of the sphere trace start backwards according to the current FGameplayTag set on the global FGameplayTag variable "Parkour_Climb_Style". 
		const FVector Offset_Start_1{Move_Vector_Backward(Arrow_Location, Arrow_Direction, Value_To_Offset_Sphere_Trace_Backwards)};
		//Offset the location to the sphere trace to the right or left depending on the value stored in the local double variable "Horizontal_Move_Direction_Update".
		//The value stored in this variable is dependent on the input from the controller.
		const FVector Offset_Start_2{Move_Vector_Right(Offset_Start_1, Arrow_Direction, Horizontal_Move_Direction_Update)};
		//During each iteration of the loop move the sphere trace down by 10 units.
		const FVector Start{Move_Vector_Down(Offset_Start_2, Index_1 * 10)};
		//The sphere trace should have a lenght of 80.
		const FVector End {Move_Vector_Forward(Start, Arrow_Direction, 80)};

		UKismetSystemLibrary::SphereTraceSingleForObjects(
			this,
			Start,
			End,
			5.f,
			Parkour_Climbing_Detect_Wall_Trace_Types,
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::ForOneFrame,
			Parkour_Climbing_Detect_Wall_Hit_Result,
			false
			);

		//If ".bStartPenetrating" is true "Stop_Parkour_Climb_Movement_Immediately_And_Reset_Movement_Input_Variables", "Reset_Movement_Input_Variables" and "return" should be called. This is because the character
		//is too close to the surface of the wall and in result can't continue shimmying.  
		if(Parkour_Climbing_Detect_Wall_Hit_Result.bStartPenetrating)
		{
			Stop_Parkour_Climb_Movement_Immediately_And_Reset_Movement_Input_Variables();
			return false;
		}

		//If ".bStartPenetrating" is false, then continue to check if the HitResult of the current loop iteration has a ".bBlockingHit". 
		else
		{
			//If the HitResult of the outer for loop iteration has a blocking hit continue to develop another sphere trace algorithem which will obtain the 
			//the "Parkour_Climbing_Wall_Top_Result".
			if(Parkour_Climbing_Detect_Wall_Hit_Result.bBlockingHit)
			{
				//Replace the current reversed wall normal stored in the global variable "Reversed_Front_Wall_Normal_Z" with the Reversed Wall Normal which may be calculated 
                //with the FHitResult from the outer for loop. This will be then new "Reversed_Front_Wall_Normal_Z" for as long as the character is shimmying.
				Reversed_Front_Wall_Normal_Z = Reverse_Wall_Normal_Rotation_Z(Parkour_Climbing_Detect_Wall_Hit_Result.ImpactNormal);

				//This for loop generates a sphere trace which shoots downwards from a forward and upward offset position of the "Parkour_Climbing_Detect_Wall_Hit_Result.ImpactPoint". The goal of this for loop is to perform a ray trace which starts inside
				//of the wall (infront and slightly upwards from the local FHitResult variable "Parkour_Climbing_Detect_Wall_Hit_Result"). With every iteration of the for loop the ray trace starting position will move up by five units. This is to obtain the new  
				//"Parkour_Climbing_Wall_Top_Result" (the const reference input parameter). If ".bStartPenetrating" is true and the outer for loop and this inner for loop are both complete then "Stop_Parkour_Climb_Movement_Immediately_And_Reset_Movement_Input_Variables" 
				//and "return" will be called. In this outcome there is no wall top to be obtained and in result the character will not be able to move via the "Move_Character_To_New_Climb_Position_Interpolation_Settings" function. Said function uses the "Parkour_Climbing_Wall_Top_Result"
				//as the location to to move the character (on the "Z" axis) while simultaneously using "Parkour_Climbing_Detect_Wall_Hit_Result" as the location to move the characer to on the "X" and "Y" axis. Also, if ".bStartPenetrating" is true and both the outer and inner for loop are
				//complete then this means that there is no room above the hands for the character to shimmy. Due to the starting point of the sphere trace raising on the "Z" axis with every iteration of the for loop until until the threshold is reached the character will have the opportunity 
				//to shimmy up and down surface ledges. Once there is a blocking hit on this trace "break" will be called as the "Parkour_Climbing_Wall_Top_Result" will be stored, hence ther is room for the character to shimmy. Remember, this trace happens at the same "Y" location as the 
				//location stored in the FHitResult "Parkour_Climbing_Detect_Wall_Hit_Result". This means when the character is moveing to the left this sphere trace will be on the left side of the arrow actor (which is just above the character) and when the character is moveing to the left 
				//the sphere trace will be on the left side of the arrow actor. This is calculated using the local double variable "Horizontal_Move_Direction_Update" which is the global double variable "Right_Left_Movement_Value" multiplied by the input parameter 
				//"Right_Left_Movement_Value_Multiplier".

				int Index_2{};
				for(Index_2; Index_2 <= 7; Index_2++)
				{
					//Offset the vector from the hit result of the outer for loop "Parkour_Climbing_Detect_Wall_Hit_Result.ImpactPoint" forward (in front of the character) by two units. At this location the vector will be inisde of the wall.
					const FVector Nested_For_Loop_Offset_Start_1{Move_Vector_Forward(Parkour_Climbing_Detect_Wall_Hit_Result.ImpactPoint, Reversed_Front_Wall_Normal_Z, 2.f)};
					//Offset the vector from the hit result of the outer for loop "Parkour_Climbing_Detect_Wall_Hit_Result.ImpactPoint" up by 5 units. At this location the vecor will be a bit under the top surface of the wall (still inside the wall).
					const FVector Nested_For_Loop_Offset_Start_2{Move_Vector_Up(Nested_For_Loop_Offset_Start_1, 5.f)};
					//With each iteration of this inner nested for loop move the start location up by five units from its previous start location.
					const FVector Nested_For_Loop_Start{Move_Vector_Up(Nested_For_Loop_Offset_Start_2, Index_2 * 5)};
					//The height of the sphere trace will be 30 units.
					const FVector Nested_For_Loop_End{Move_Vector_Down(Nested_For_Loop_Start, 30.f)};

					UKismetSystemLibrary::SphereTraceSingleForObjects(
						this,
						Nested_For_Loop_Start,
						Nested_For_Loop_End,
						3.f,
						Parkour_Climbing_Wall_Top_Result_Trace_Types,
						false,
						TArray<AActor*>(),
						EDrawDebugTrace::ForOneFrame,
						Parkour_Climbing_Wall_Top_Result,
						false
						);

					//If ".bStartPenetrating" is true and the outer for loop and this inner for loop are both complete then "Stop_Parkour_Climb_Movement_Immediately_And_Reset_Movement_Input_Variables" and "return" will be called. 
					//In this outcome there is no wall top to be obtained and in result the character will not be able to move via the "Move_Character_To_New_Climb_Position_Interpolation_Settings" function. Otherwise "continue" will be called.
					//This is because the threshold for the maximum height that the character can reach up to shimmy up a ledge hasn't been reahced. Otherwise "continue" will be called so that the the wall may continue to be analyzed.
					if(Parkour_Climbing_Wall_Top_Result.bStartPenetrating)
					{
						if(Index_1 == 7 && Index_2 == 7)
						{
							Stop_Parkour_Climb_Movement_Immediately_And_Reset_Movement_Input_Variables();
							return false;
						}
						
						else
						{
							continue;
						}
					}

					//If ".bStartPenetrating" is false, check to see if the there is a blocking hit on the FHitResult 'Parkour_Climbing_Wall_Top_Result". If there is a blocking hit, return out of this function as the wall has been analyzed and in result the
					//"Parkour_Climbing_Detect_Wall_Hit_Result" and the "Parkour_Climbing_Wall_Top_Result" has both been obtained. If there is no blocking hit for the FHitResult "Parkour_Climbing_Wall_Top_Result", then check to see if both the outer and inner
					//for loop are complete. If both for loops are complete, call "Stop_Parkour_Climb_Movement_Immediately_And_Reset_Movement_Input_Variables" and "return". Otherwise call "continue" so the wall may be be analyzed further.
					else
					{
						if(Parkour_Climbing_Wall_Top_Result.bBlockingHit)
						{
							return true;
						}

						else
						{
							if(Index_1 == 7 && Index_2 == 7)
							{
								Stop_Parkour_Climb_Movement_Immediately_And_Reset_Movement_Input_Variables();
								return false;
							}
						
							else
							{
								continue;
							}
						}
					}
				}
			}

			//If the HitResult of the outer loop iteration does not have a blocking hit check to see if the loop has completed. If this is the case call "Stop_Parkour_Climb_Movement_Immediately_And_Reset_Movement_Input_Variables"
			//and "return". Otherwise call "continue".
			else
			{
				if(Index_1 == 7)
				{
					Stop_Parkour_Climb_Movement_Immediately_And_Reset_Movement_Input_Variables();
					return false;
				}
					
				else
				{
					continue;
				}
			}
		}
	}

	return false;
}

bool UCustom_Movement_Component::Parkour_Climb_State_Are_There_Obstacles_On_Sides_Of_Hands(const FVector& Starting_Impact_Point)
{
	/*This function develops line traces from an offset location above the FHitResult "Parkour_Climbing_Wall_Top_Result" (declared as a local variable within the function "Parkour_Climb_Handle_Shimmying_Movement"). Because said line traces
	use "Parkour_Climbing_Wall_Top_Result" as their starting point, the line traces will be generated on the side of the arrow actor that the character is shimmying. So if the character is moving to the left, the line traces will be on the
	left side of the arrow actor (offset just above the hands). Same goes for the right side. This is because "Parkour_Climbing_Wall_Top_Result" uses "Parkour_Climbing_Detect_Wall_Hit" as its starting point and "Parkour_Climbing_Detect_Wall_Hit 
	is generated on the side of the arrow actor which the character is moving. Said location is calculated by moving the vector to the right side of the arrow actor (or left if the value is negative) by using the helper function "Move_Vector_Right" 
	(passing in the "Right_Left_Movement_Value" multiplied by the "Right_Left_Movement_Value_Multiplier"). With each iteration of the for loop performed the line trace start point will be raised up five units from its previous start location. However, the line
	traces will only be generated if there is a blocking hit on the line trace which is generated on the first iteration of the for loop. If the first line trace which is generated does not have a blocking hit then this means there is no obstacle 
	on the respective side of the character's hands (the hand on the side of the body in which the character is shimmying). If there is a blocking hit on the first line trace which is generated, then the for loop will continue to generate more line
	traces which will all start five units above the previous line trace. This will happen until the threshold is reahced (which means that there is a obsticle on the side of the characters hands) and in result "true" should be returned 
	(which results in "Stop_Parkour_Climb_Movement_Immediately_And_Reset_Movement_Input_Variables" and "return" being called within the scope of the if chech which this function is used in within the function "Parkour_Climb_Handle_Shimmying_Movement") or until there
	is no blocking hit on the most recent line trace. In which case "break" will be called.*/
	
	//Offset the starting location of the line trace two units above the input parameter "Starting_Impact_Point". This location will be the "Parkour_Climbing_Wall_Top_Result".
	const FVector Offset_Starting_Impact_Point{Move_Vector_Up(Starting_Impact_Point, 2.f)};
	
	int Index{};
	FHitResult Parkour_Climbing_Check_Sides{};

	for(Index; Index <= 5; Index++)
	{
		const FVector Start{Move_Vector_Up(Offset_Starting_Impact_Point, Index * 5)};
		const FVector End{Move_Vector_Right(Start, Reversed_Front_Wall_Normal_Z, Right_Left_Movement_Value * 45.f)};

		UKismetSystemLibrary::LineTraceSingleForObjects(
			this,
			Start,
			End,
			Parkour_Climbing_Check_Sides_Of_Hands_Trace_Types,
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::ForOneFrame,
			Parkour_Climbing_Check_Sides,
			false
			);
		
		//Check to see if the line traced generated has a blocking hit. If it does have a blocking hit, check to see if the for loop is complete. If the for loop is complete, this means the threshold has been reached and there is a obstacle on the respective
		//side of the character's hands. in this case true will be returned which will lead to "Stop_Parkour_Climb_Movement_Immediately_And_Reset_Movement_Input_Variables" and "return" being called in the scope of the if chech that uses this function within the
		//fucntion "Parkour_Climb_Handle_Shimmying_Movement". If the for loop is not complete "continue" is called so the area on the respective side of the character's hands may be analyzed for obstacles.

		//IF there is no blocking hit on any line trace which is generated before the threshold is reahced "false" will be retruned.
		if(Parkour_Climbing_Check_Sides.bBlockingHit)
		{
			if(Index == 5)
			{
				return true;
			}
			
			else
			{
				continue;
			}
		}

		else
		{
			return false;
		}
	}

	//Default return value. This line is needed to meet the requirements of this function return type.
	return true;
}

bool UCustom_Movement_Component::Parkour_Climb_State_Are_There_Obstacles_On_Sides_Of_Body(const FVector& Movemement_Impact_Location) const
{
	/*This function develops a dynamic a ray trace on the side of the characters body (the same side the character is shimmying) to determine if there is a obsticle which the character shouldn't be able to shimmy across.
	The starting location for the capsule trace is ofset to to the right side or left side (depending which direction the character is moving), as well as down depending on the "Parkour_Climb_Style". which the character is currently 
	in.*/

	//Dynamically set the height of the capsule trace based on the "Parkour_Climb_Style"
	const float Capsule_Trace_Dynamic_Down_Offset_And_Height{Select_Value_Based_On_Climb_Style(Parkour_Climb_Style, 55.f, 90.f)};
	//Dynamically set the start location of the capsule trace based on the "Parkour_Climb_Style"
	const float Capsule_Trace_Dynamic_Start{Select_Value_Based_On_Climb_Style(Parkour_Climb_Style, 30.f, 7.f)};
	////Dynamically set the end location of the capsule trace based on the "Parkour_Climb_Style"
	const float Capsule_Trace_Dynamic_End{Select_Value_Based_On_Climb_Style(Parkour_Climb_Style, 40.f, 15.f)};
	
	FHitResult Out_Hit{};
	
	//The arrow actor is always facing the same direction as the character.
	const FRotator Direction_Character_Is_Facing{Character_Direction_Arrow->GetActorRotation()};
	
	//The sphere trace is offset to the right or left side of the arrow actor. Depending on the direction the character is shimmying (based on "Right_Left_Movement_Value" which is set within the 
	//function &UCustom_Movement_Component::Add_Movement_Input).
	const FVector Offset_Vector_To_The_Right{Move_Vector_Right(Movemement_Impact_Location, Direction_Character_Is_Facing, Right_Left_Movement_Value * 35.f)};
	
	//The sphere trace is offset down dynamically from the "Offset_Vector_To_The_Right" location.
	const FVector Offset_Vector_Down{Move_Vector_Down(Offset_Vector_To_The_Right, Capsule_Trace_Dynamic_Down_Offset_And_Height)};
	
	//The start location of the capsule trace location is set dynamically based on the "Parkour_Climb_Style".
	const FVector Start{Move_Vector_Backward(Offset_Vector_Down, Direction_Character_Is_Facing, Capsule_Trace_Dynamic_Start)};
	//The end location of the capsule trace location is set dynamically based on the "Parkour_Climb_Style"
	const FVector End{Move_Vector_Backward(Start, Direction_Character_Is_Facing, Capsule_Trace_Dynamic_End)};

	UKismetSystemLibrary::CapsuleTraceSingleForObjects(
		this,
		Start,
		End,
		5.f,
		Capsule_Trace_Dynamic_Down_Offset_And_Height,
		Parkour_Climbing_Check_Sides_Of_Body_Trace_Types,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::ForOneFrame,
		Out_Hit,
		false
		);
	
	//If there is no a blocking hit "false" will be returned and the character will be allowed to shimmy in that direction.
	if(!Out_Hit.bBlockingHit)
	return false;

	//If there is a blocking hit "true" will be returned and in result "Stop_Parkour_Climb_Movement_Immediately_And_Reset_Movement_Input_Variables" and "return" will be called within the scope of 
	//the if check in which this function is used within "&UCustom_Movement_Component::Parkour_Climb_Handle_Shimmying_Movement"
	else
	return true;
}

void UCustom_Movement_Component::Parkour_Climb_Initialize_IK_Hands(const bool& bIs_Left_Hand)
{
	/*This function handles the implementation of the IK hands when "Parkour_State" changes to "Parkour.State.Climb". When the input parameter if set to "true" when this
	function is called the implementation of this function will handle the left hand when it is set to false the right hand will be handled.*/
	
	if(Parkour_State != FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Ready.To.Climb"))))
	return;

	if(!Initialize_Parkour_IK_Limbs_Hit_Result.bBlockingHit)
	return;

	int Index_1{};
	const int Index_1_Multiplier{Index_1 * 2};
	const int Distance_To_Offset_Hands_From_Initialize_Parkour_IK_Limbs_Hit_Result{28};
	int Select_Left_Or_Right_Hand_Value{};

	if(bIs_Left_Hand == true)
	Select_Left_Or_Right_Hand_Value = 1;

	else
	Select_Left_Or_Right_Hand_Value = -1;

	const int Move_Vector_For_Right_Or_Left_Hand_Value{Select_Left_Or_Right_Hand_Value * (Index_1_Multiplier - Distance_To_Offset_Hands_From_Initialize_Parkour_IK_Limbs_Hit_Result)};

	FHitResult Initialize_Parkour_Shimmying_IK_Hands_Detect_Wall_Hit_Result{};

	FRotator Hand_Shimmy_Rotation{};

	FVector Hand_Shimmy_Location{};

	for(Index_1; Index_1 <= 4; Index_1++)
	{
		const FVector Move_Right_Offset{Move_Vector_Right(Initialize_Parkour_IK_Limbs_Hit_Result.ImpactPoint, Reversed_Front_Wall_Normal_Z, Move_Vector_For_Right_Or_Left_Hand_Value)};
		
		const FVector Start_1{Move_Vector_Backward(Move_Right_Offset, Reversed_Front_Wall_Normal_Z, 30.f)};
		const FVector End_1{Move_Vector_Forward(Start_1, Reversed_Front_Wall_Normal_Z, 70.f)};

		UKismetSystemLibrary::SphereTraceSingleForObjects(
			this,
			Start_1,
			End_1,
			5.f,
			Parkour_Shimmying_Initialize_IK_Hands_Detect_Wall_Trace_Types,
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::ForDuration,
			Initialize_Parkour_Shimmying_IK_Hands_Detect_Wall_Hit_Result,
			false
		);

		if(Initialize_Parkour_Shimmying_IK_Hands_Detect_Wall_Hit_Result.bBlockingHit)
		Reversed_Front_Wall_Normal_Z = Reverse_Wall_Normal_Rotation_Z(Initialize_Parkour_Shimmying_IK_Hands_Detect_Wall_Hit_Result.ImpactNormal);

		else
		{
			if(Index_1 == 4)
			return;

			else
			continue;
		}

		const FRotator Left_Hand_Shimmy_Rotation{FRotator(50, 0, 270)};
		const FRotator Right_Hand_Shimmy_Rotation{FRotator(230, 0, 270)};

		if(bIs_Left_Hand == true)
		Hand_Shimmy_Rotation = Reversed_Front_Wall_Normal_Z + Left_Hand_Shimmy_Rotation;

		else
		Hand_Shimmy_Rotation = Reversed_Front_Wall_Normal_Z + Right_Hand_Shimmy_Rotation;

		
		
		int Index_2{};

		FHitResult Initialize_Parkour_Shimmying_IK_Hands_Wall_Top_Hit_Result{};

		for(Index_2; Index_2 <= 7; Index_2++)
		{
			const FVector Move_Forward_Offset{Move_Vector_Forward(Initialize_Parkour_Shimmying_IK_Hands_Detect_Wall_Hit_Result.ImpactPoint, Reversed_Front_Wall_Normal_Z, 2.f)};

			const FVector Start_2{Move_Vector_Up(Move_Forward_Offset, Index_2 * 5.f)};
			const FVector End_2{Move_Vector_Down(Start_2, 30.f)};

			UKismetSystemLibrary::SphereTraceSingleForObjects(
				this,
				Start_2,
				End_2,
				3.f,
				Parkour_Shimmying_Initialize_IK_Hands_Wall_Top_Trace_Types,
				false,
				TArray<AActor*>(),
				EDrawDebugTrace::ForDuration,
				Initialize_Parkour_Shimmying_IK_Hands_Wall_Top_Hit_Result,
				false
			);

			if(Initialize_Parkour_Shimmying_IK_Hands_Wall_Top_Hit_Result.bBlockingHit && !Initialize_Parkour_Shimmying_IK_Hands_Wall_Top_Hit_Result.bStartPenetrating)
			{
				Hand_Shimmy_Location = FVector(Initialize_Parkour_Shimmying_IK_Hands_Detect_Wall_Hit_Result.ImpactPoint.X,
												  		Initialize_Parkour_Shimmying_IK_Hands_Detect_Wall_Hit_Result.ImpactPoint.Y, 
												  		Initialize_Parkour_Shimmying_IK_Hands_Wall_Top_Hit_Result.ImpactPoint.Z);
				break;
			}
			
			else
			{
				if(Index_1 == 4 && Index_2 == 7)
				return;

				else
				continue;
			}
		}	

		break;
	}	

	if(bIs_Left_Hand == true)
	{
		Parkour_Interface->Execute_Set_Left_Hand_Shimmy_Location(Anim_Instance, Hand_Shimmy_Location);
		Parkour_Interface->Execute_Set_Left_Hand_Shimmy_Rotation(Anim_Instance, Hand_Shimmy_Rotation);
	}

	else
	{
		Parkour_Interface->Execute_Set_Right_Hand_Shimmy_Location(Anim_Instance, Hand_Shimmy_Location);
		Parkour_Interface->Execute_Set_Right_Hand_Shimmy_Rotation(Anim_Instance, Hand_Shimmy_Rotation);
	}
}

void UCustom_Movement_Component::Parkour_Climb_Dynamic_IK_Hands(const bool& bIs_Left_Hand)
{
	if(Parkour_State != FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))))
	return;
	
	
	FVector Hand_Shimmy_Location{};

	FRotator Hand_Shimmy_Rotation{};


	int Index_1{};

	FHitResult Parkour_Shimmying_Dynamic_IK_Hands_Detect_Wall_Hit_Result{};

	FRotator Parkour_Shimmying_Dynamic_IK_Hands_Reversed_Front_Wall_Normal_Z{};

	for(Index_1; Index_1 <= 4; Index_1++)
	{
		const FRotator Direction_Character_Is_Facing{Character_Direction_Arrow->GetActorRotation()};

		FVector Select_Left_or_Right_IK_Hand{};

		if(bIs_Left_Hand == true)
		Select_Left_or_Right_IK_Hand = Mesh->GetSocketLocation(FName(TEXT("ik_hand_l")));

		else
		Select_Left_or_Right_IK_Hand = Mesh->GetSocketLocation(FName(TEXT("ik_hand_r")));

		const FVector Offset_Start_Down{Move_Vector_Down(Select_Left_or_Right_IK_Hand, Index_1 * 5)};
		const FVector Start_1{Move_Vector_Backward( Offset_Start_Down, Direction_Character_Is_Facing, 30.f)};
		const FVector End_1{Move_Vector_Forward(Start_1, Direction_Character_Is_Facing, 70.f)};

		UKismetSystemLibrary::SphereTraceSingleForObjects(
			this,
			Start_1,
			End_1,
			10.f,
			Parkour_Shimmying_Dynamic_IK_Hands_Detect_Wall_Trace_Types,
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::ForOneFrame,
			Parkour_Shimmying_Dynamic_IK_Hands_Detect_Wall_Hit_Result,
			false
		);

		if(Parkour_Shimmying_Dynamic_IK_Hands_Detect_Wall_Hit_Result.bBlockingHit && !Parkour_Shimmying_Dynamic_IK_Hands_Detect_Wall_Hit_Result.bStartPenetrating)
		{
			Parkour_Shimmying_Dynamic_IK_Hands_Reversed_Front_Wall_Normal_Z = Reverse_Wall_Normal_Rotation_Z(Parkour_Shimmying_Dynamic_IK_Hands_Detect_Wall_Hit_Result.ImpactNormal);
			
			const FRotator Left_Hand_Shimmy_Rotation{FRotator(50, 0, 270)};
			const FRotator Right_Hand_Shimmy_Rotation{FRotator(230, 0, 270)};

			if(bIs_Left_Hand == true)
			Hand_Shimmy_Rotation = Parkour_Shimmying_Dynamic_IK_Hands_Reversed_Front_Wall_Normal_Z + Left_Hand_Shimmy_Rotation;

			else
			Hand_Shimmy_Rotation = Parkour_Shimmying_Dynamic_IK_Hands_Reversed_Front_Wall_Normal_Z + Right_Hand_Shimmy_Rotation;


			int Index_2{};

			FHitResult Parkour_Shimmying_Dynamic_IK_Hands_Wall_Top_Hit_Result{};

			for(Index_2; Index_2 <= 7; Index_2++)
			{
				const FVector Offset_Vector_Forward{Move_Vector_Forward(Parkour_Shimmying_Dynamic_IK_Hands_Detect_Wall_Hit_Result.ImpactPoint, Parkour_Shimmying_Dynamic_IK_Hands_Reversed_Front_Wall_Normal_Z, 2.f)};
				const FVector Start_2{Move_Vector_Up(Offset_Vector_Forward, Index_2 * 5.f)};
				const FVector End_2{Move_Vector_Down(Start_2, 70.f)};

				UKismetSystemLibrary::SphereTraceSingleForObjects(
					this,
					Start_2,
					End_2,
					3.f,
					Parkour_Shimmying_Dynamic_IK_Hands_Wall_Top_Trace_Types,
					false,
					TArray<AActor*>(),
					EDrawDebugTrace::ForOneFrame,
					Parkour_Shimmying_Dynamic_IK_Hands_Wall_Top_Hit_Result,
					false
				);

				if(Parkour_Shimmying_Dynamic_IK_Hands_Wall_Top_Hit_Result.bBlockingHit && !Parkour_Shimmying_Dynamic_IK_Hands_Wall_Top_Hit_Result.bStartPenetrating)
				{
					if(Parkour_Climb_Style == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Climb.Style.Braced.Climb"))))
					{
						if(Parkour_Direction == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Left"))))
						{
							Hand_Shimmy_Location = FVector(Parkour_Shimmying_Dynamic_IK_Hands_Detect_Wall_Hit_Result.ImpactPoint.X,
										   				   Parkour_Shimmying_Dynamic_IK_Hands_Detect_Wall_Hit_Result.ImpactPoint.Y,
										   		           Parkour_Shimmying_Dynamic_IK_Hands_Wall_Top_Hit_Result.ImpactPoint.Z - 3.f);

							break;
						}

						else if(Parkour_Direction == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Right"))))
						{
							Hand_Shimmy_Location = FVector(Parkour_Shimmying_Dynamic_IK_Hands_Detect_Wall_Hit_Result.ImpactPoint.X,
										                   Parkour_Shimmying_Dynamic_IK_Hands_Detect_Wall_Hit_Result.ImpactPoint.Y,
										   				   Parkour_Shimmying_Dynamic_IK_Hands_Wall_Top_Hit_Result.ImpactPoint.Z - 10.f);

							break;
						}

						else if(Parkour_Direction == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.None"))))
						{
							Hand_Shimmy_Location = FVector(Parkour_Shimmying_Dynamic_IK_Hands_Detect_Wall_Hit_Result.ImpactPoint.X - 3,
										   			       Parkour_Shimmying_Dynamic_IK_Hands_Detect_Wall_Hit_Result.ImpactPoint.Y,
										   				   Parkour_Shimmying_Dynamic_IK_Hands_Wall_Top_Hit_Result.ImpactPoint.Z + 3);
							break;
						}
					}

					else if(Parkour_Climb_Style == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Climb.Style.FreeHang"))))
					{
						if(Parkour_Direction == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Left"))))
						{
							Hand_Shimmy_Location = FVector(Parkour_Shimmying_Dynamic_IK_Hands_Detect_Wall_Hit_Result.ImpactPoint.X - 5,
										   				   Parkour_Shimmying_Dynamic_IK_Hands_Detect_Wall_Hit_Result.ImpactPoint.Y,
										   				   Parkour_Shimmying_Dynamic_IK_Hands_Wall_Top_Hit_Result.ImpactPoint.Z - 7.f);

							break;
						}

						else if(Parkour_Direction == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Right"))))
						{
							Hand_Shimmy_Location = FVector(Parkour_Shimmying_Dynamic_IK_Hands_Detect_Wall_Hit_Result.ImpactPoint.X -5,
										  				   Parkour_Shimmying_Dynamic_IK_Hands_Detect_Wall_Hit_Result.ImpactPoint.Y,
										   				   Parkour_Shimmying_Dynamic_IK_Hands_Wall_Top_Hit_Result.ImpactPoint.Z - 7.f);

							break;
						}

						else if(Parkour_Direction == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.None"))))
						{
							Hand_Shimmy_Location = FVector(Parkour_Shimmying_Dynamic_IK_Hands_Detect_Wall_Hit_Result.ImpactPoint.X - 3,
										   				   Parkour_Shimmying_Dynamic_IK_Hands_Detect_Wall_Hit_Result.ImpactPoint.Y,
										   				   Parkour_Shimmying_Dynamic_IK_Hands_Wall_Top_Hit_Result.ImpactPoint.Z - 3);
							break;
						}
					}
				}

				else
				{
					if(Index_1 == 4 && Index_2 == 7)
					{
						Reset_Parkour_IK_Hands(bIs_Left_Hand);
						return;
					}

					else if(Index_2 == 7)
					{
						Reset_Parkour_IK_Hands(bIs_Left_Hand);
						return;
					}
			
					else
					{
						continue;
					}
				}
			}

			break;
		}
		
		else
		{
			if(Index_1 == 4)
			{
				Reset_Parkour_IK_Hands(bIs_Left_Hand);
				return;
			}
			
			else
			{
				continue;
			}
		}
	}

	if(bIs_Left_Hand == true)
	{
		Parkour_Interface->Execute_Set_Left_Hand_Shimmy_Location(Anim_Instance, Hand_Shimmy_Location);
		Parkour_Interface->Execute_Set_Left_Hand_Shimmy_Rotation(Anim_Instance, Hand_Shimmy_Rotation);
		return;
	}

	else
	{
		Parkour_Interface->Execute_Set_Right_Hand_Shimmy_Location(Anim_Instance, Hand_Shimmy_Location);
		Parkour_Interface->Execute_Set_Right_Hand_Shimmy_Rotation(Anim_Instance, Hand_Shimmy_Rotation);
		return;
	}
}

void UCustom_Movement_Component::Parkour_Climb_Initialize_IK_Feet(const bool& bIs_Left_Foot)
{
	if(Parkour_Climb_Style != FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Climb.Style.Braced.Climb"))))
	return;

	FVector Offset_Vector_Down{};
	
	if(bIs_Left_Foot)
	Offset_Vector_Down = Move_Vector_Down(Initialize_Parkour_IK_Limbs_Hit_Result.ImpactPoint, 125.f);

	else
	Offset_Vector_Down = Move_Vector_Down(Initialize_Parkour_IK_Limbs_Hit_Result.ImpactPoint, 110.f);

	
	
	FVector Offset_Vector_To_Right_Or_Left{};

	if(bIs_Left_Foot == true)
	Offset_Vector_To_Right_Or_Left = Move_Vector_Left(Offset_Vector_Down, Reversed_Front_Wall_Normal_Z, 20.f);

	else
	Offset_Vector_To_Right_Or_Left = Move_Vector_Right(Offset_Vector_Down, Reversed_Front_Wall_Normal_Z, 20.f);

	int Index{};

	FHitResult Initialize_Parkour_Shimmying_IK_Feet{};

	FRotator Initialize_Parkour_Shimmying_IK_Feet_Reversed_Wall_Normal_On_Z{};

	FVector Feet_Shimmy_Location{};

	FRotator Feet_Shimmy_Rotation{};

	for(Index; Index <= 2; Index++)
	{
		const FVector Offset_Start{Move_Vector_Up(Offset_Vector_To_Right_Or_Left, Index * 5.f)};
		const FVector Start{Move_Vector_Backward(Offset_Start, Reversed_Front_Wall_Normal_Z, 30.f)};
		const FVector End{Move_Vector_Forward(Start, Reversed_Front_Wall_Normal_Z, 40.f)};

		UKismetSystemLibrary::SphereTraceSingleForObjects(
			this,
			Start,
			End,
			7.f,
			Parkour_Shimmying_Initialize_IK_Feet_Trace_Types,
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::ForDuration,
			Initialize_Parkour_Shimmying_IK_Feet,
			false
		);

		const FRotator Left_Foot_Shimmy_Rotation{FRotator(110, -10, 100)};
		const FRotator Right_Foot_Shimmy_Rotation{FRotator(-55, 0, 95)};

		if(Initialize_Parkour_Shimmying_IK_Feet.bBlockingHit && !Initialize_Parkour_Shimmying_IK_Feet.bStartPenetrating)
		{
			Initialize_Parkour_Shimmying_IK_Feet_Reversed_Wall_Normal_On_Z = Reverse_Wall_Normal_Rotation_Z(Initialize_Parkour_Shimmying_IK_Feet.ImpactNormal);
			
			Feet_Shimmy_Location = Move_Vector_Backward(Initialize_Parkour_Shimmying_IK_Feet.ImpactPoint, Initialize_Parkour_Shimmying_IK_Feet_Reversed_Wall_Normal_On_Z, 17.f);
			
			if(bIs_Left_Foot == true)
			Feet_Shimmy_Rotation = Initialize_Parkour_Shimmying_IK_Feet_Reversed_Wall_Normal_On_Z + Left_Foot_Shimmy_Rotation;

			else
			Feet_Shimmy_Rotation = Initialize_Parkour_Shimmying_IK_Feet_Reversed_Wall_Normal_On_Z + Right_Foot_Shimmy_Rotation;

			break;
		}

		else
		{
			if(Index == 2)
			return;

			else
			continue;
		}
	}

	if(bIs_Left_Foot == true)
	{
		Parkour_Interface->Execute_Set_Left_Foot_Shimmy_Location(Anim_Instance, Feet_Shimmy_Location);
		Parkour_Interface->Execute_Set_Left_Foot_Shimmy_Rotation(Anim_Instance, Feet_Shimmy_Rotation);
	}
	
	else
	{
		Parkour_Interface->Execute_Set_Right_Foot_Shimmy_Location(Anim_Instance, Feet_Shimmy_Location);
		Parkour_Interface->Execute_Set_Right_Foot_Shimmy_Rotation(Anim_Instance, Feet_Shimmy_Rotation);
	}
	
}

void UCustom_Movement_Component::Parkour_Climb_Dynamic_IK_Feet(const bool& bIs_Left_Foot)
{
	if(Parkour_Climb_Style != FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Climb.Style.Braced.Climb"))))
	return;

	//Get the socket location for the left or right IK foot bone.
	FVector Left_Or_Right_IK_Foot_Socket_Location{};
	if(Mesh)
	{
		if(bIs_Left_Foot)
		Left_Or_Right_IK_Foot_Socket_Location = Mesh->GetSocketLocation(FName(TEXT("ik_foot_l")));

		else
		Left_Or_Right_IK_Foot_Socket_Location = Mesh->GetSocketLocation(FName(TEXT("ik_foot_r")));
	}
	
	//Get the socket location for the left or right ik hand bone.
	FVector Left_Or_Right_Hand_Socket_Location{};
	if(Mesh)
	{
		if(bIs_Left_Foot)
		Left_Or_Right_Hand_Socket_Location = Mesh->GetSocketLocation(FName(TEXT("ik_hand_l")));

		else
		Left_Or_Right_Hand_Socket_Location = Mesh->GetSocketLocation(FName(TEXT("ik_hand_r")));
	}

	//Offset the vector from the corresponding hand down to the level of where the feet should be.
	FVector Offset_Vector_Down{};

	if(bIs_Left_Foot)
	Offset_Vector_Down = FVector(Left_Or_Right_IK_Foot_Socket_Location.X, Left_Or_Right_IK_Foot_Socket_Location.Y, Left_Or_Right_Hand_Socket_Location.Z - 125.f);

	else
	Offset_Vector_Down = FVector(Left_Or_Right_IK_Foot_Socket_Location.X, Left_Or_Right_IK_Foot_Socket_Location.Y, Left_Or_Right_Hand_Socket_Location.Z - 110.f);;
	

	int Index{};
	FHitResult Parkour_Shimmying_Dynamic_IK_Feet_Detect_Wall_Hit_Result{};
	
	FVector Feet_Shimmy_Location{};
	FRotator Feet_Shimmy_Rotation{};

	for(Index; Index <= 2; Index++)
	{
		//Direction the character is facing
		const FRotator Direction_Character_Is_Facing{Character_Direction_Arrow->GetActorRotation()};

		//With each iteration of the for loop move the vector up (as long as there is no "bBlockingHit").
		const FVector Offset_Vector_Up_With_Each_Iteration_Of_For_Loop{Move_Vector_Up(Offset_Vector_Down, Index * 5.f)};
		//Start location of the ray cast
		const FVector Start{Move_Vector_Backward(Offset_Vector_Up_With_Each_Iteration_Of_For_Loop, Direction_Character_Is_Facing, 30.f)};
		//End location of the ray cast.
		const FVector End{Move_Vector_Forward(Start, Direction_Character_Is_Facing, 70.f)};

		UKismetSystemLibrary::SphereTraceSingleForObjects(
			this,
			Start,
			End,
			10.f,
			Parkour_Shimmying_Dynamic_IK_Feet_Detect_Wall_Trace_Types,
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::ForOneFrame,
			Parkour_Shimmying_Dynamic_IK_Feet_Detect_Wall_Hit_Result,
			false
		);

		const FRotator Left_Foot_Shimmy_Rotation{FRotator(110, -10, 100)};
		const FRotator Right_Foot_Shimmy_Rotation{FRotator(-55, 0, 95)};
		
		//Check to see if "bStartPenetrating" is false and there is a blocking hit. If this check passes assign the location of the impact point to the local FVector variable "Foot_Shimmy_Location" then
		//break out of the for loop. If this check fails check to see if the for loop has reached the maxiumim Index value. If so, Index return out of the function otherwise "continue".
		if(!Parkour_Shimmying_Dynamic_IK_Feet_Detect_Wall_Hit_Result.bStartPenetrating && Parkour_Shimmying_Dynamic_IK_Feet_Detect_Wall_Hit_Result.bBlockingHit)
		{
			const FRotator Parkour_Shimmying_IK_Feet_Reversed_Wall_Normal_On_Z{Reverse_Wall_Normal_Rotation_Z(Parkour_Shimmying_Dynamic_IK_Feet_Detect_Wall_Hit_Result.ImpactNormal)};

			Feet_Shimmy_Location = Move_Vector_Backward(Parkour_Shimmying_Dynamic_IK_Feet_Detect_Wall_Hit_Result.ImpactPoint, Parkour_Shimmying_IK_Feet_Reversed_Wall_Normal_On_Z, 17.f);


			if(bIs_Left_Foot)
			Feet_Shimmy_Rotation = Parkour_Shimmying_IK_Feet_Reversed_Wall_Normal_On_Z + Left_Foot_Shimmy_Rotation;
			
			else
			Feet_Shimmy_Rotation = Parkour_Shimmying_IK_Feet_Reversed_Wall_Normal_On_Z + Right_Foot_Shimmy_Rotation;

			break;
		}

		else
		{
			if(Index == 2)
			{	
				Reset_Parkour_IK_Feet(bIs_Left_Foot);
				return;
			}
			

			else
			{
				continue;
			}
		}
	}
	
	//Call the coresponding interface function for the appropriate limb
	if(bIs_Left_Foot)
	{
		if(Parkour_Interface && Anim_Instance)
		{
			Parkour_Interface->Execute_Set_Left_Foot_Shimmy_Location(Anim_Instance, Feet_Shimmy_Location);
			Parkour_Interface->Execute_Set_Left_Foot_Shimmy_Rotation(Anim_Instance, Feet_Shimmy_Rotation);
		}
	}

	else
	{
		if(Parkour_Interface && Anim_Instance)
		{
			Parkour_Interface->Execute_Set_Right_Foot_Shimmy_Location(Anim_Instance, Feet_Shimmy_Location);
			Parkour_Interface->Execute_Set_Right_Foot_Shimmy_Rotation(Anim_Instance, Feet_Shimmy_Rotation);
		}
	}
}

#pragma endregion

#pragma region Parkour_Core

void UCustom_Movement_Component::Parkour_State_Settings(const ECollisionEnabled::Type& Collision_Type, const EMovementMode& New_Movement_Mode, const bool& bStop_Movement_Immediately)
{
	Capsule_Component->SetCollisionEnabled(Collision_Type);
	Character_Movement->SetMovementMode(New_Movement_Mode);
	
	if(bStop_Movement_Immediately) Character_Movement->StopMovementImmediately();
}

void UCustom_Movement_Component::Set_Parkour_State_Attributes(const FGameplayTag& Current_Parkour_State)
{
	if(Current_Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Free.Roam"))))
	Parkour_State_Settings(ECollisionEnabled::QueryAndPhysics, EMovementMode::MOVE_Walking, false);

	else if(Current_Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Ready.To.Climb"))))
	Parkour_State_Settings(ECollisionEnabled::NoCollision, EMovementMode::MOVE_Flying, false);

	else if(Current_Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))))
	Parkour_State_Settings(ECollisionEnabled::NoCollision, EMovementMode::MOVE_Flying, true);

	else if(Current_Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Mantle"))))
	Parkour_State_Settings(ECollisionEnabled::NoCollision, EMovementMode::MOVE_Flying, false);

	else if(Current_Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Vault"))))
	Parkour_State_Settings(ECollisionEnabled::NoCollision, EMovementMode::MOVE_Flying, false);
}

void UCustom_Movement_Component::Set_Parkour_State(const FGameplayTag& New_Parkour_State)
{
	if(Parkour_State != New_Parkour_State)
	{
		Parkour_State = New_Parkour_State;
		if(Parkour_Interface) Parkour_Interface->Execute_Set_Parkour_State(Anim_Instance, Parkour_State);
		Set_Parkour_State_Attributes(Parkour_State);
	}
	else return;
}

void UCustom_Movement_Component::Set_Parkour_Climb_Style(const FGameplayTag& New_Climb_Style)
{
	if(Parkour_Climb_Style != New_Climb_Style)
	{
		Parkour_Climb_Style = New_Climb_Style;
		Parkour_Interface->Execute_Set_Climb_Style(Anim_Instance, Parkour_Climb_Style);
	}
	
	else return;
}

void UCustom_Movement_Component::Set_Parkour_Direction(const FGameplayTag& New_Climb_Direction)
{
	if(Parkour_Direction != New_Climb_Direction)
	{
		Parkour_Direction = New_Climb_Direction;
		Parkour_Interface->Execute_Set_Climb_Direction(Anim_Instance, Parkour_Direction);
	}
	
	else return;
}

void UCustom_Movement_Component::Set_Parkour_Action(const FGameplayTag& New_Parkour_Action)
{
	/*The goal of this function is to use the FGameplayTag input argument passed into it via the function 
	"Decide_Parkour_Action()" to check if the global FGameplaytag variable "Parkour_Action" has the same FGameplaytag 
	value. If said global variable does not have the same FGameplaytag  as what is passed in via the input argument 
	then said FGameplaytag  should be set to equal the value of what is passed in via the input argument.This is followed 
	by setting the "Parkour_Action" in the interface by using the pointer to said interface and calling the generated 
	(at compile) interface function which begins with the prefix "Execute_" "Set_Parkour_Action()". Lastly, there 
	are "if '' and "else if" checks which need to be analyzed to check whether the FGameplaytag global variable which 
	has just been set equals specific Gameplay tags. If the global FGameplaytag == "Parkour.Action.No.Action" then the 
	function "Reset_Parkour_Variables()" should be called. Otherwise other "else if" statements should follow to check 
	whether said global FGameplaytag variable "Parkour_Action"   == any of the other Parkour Action gameplay tags. 
	Whichever tag said global variable equals the function "Play_Parkour_Montage()" should be called, passing in the 
	"UParkour_Action_Data*"which holds the address to the Asset Data object that is stored inside the character Blueprint 
	within the Custom_Movement_Component.*/

	if(Parkour_Action != New_Parkour_Action)
	Parkour_Action = New_Parkour_Action;

	Parkour_Interface->Execute_Set_Parkour_Action(Anim_Instance, Parkour_Action);

	if(Parkour_Action == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.No.Action"))))
	Reset_Parkour_Variables();

	else if(Parkour_Action == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.Braced.Climb"))))
	{
		Play_Parkour_Montage(Braced_Jump_To_Climb);
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(48.f);
	}
	
	else if(Parkour_Action == FGameplayTag::RequestGameplayTag("Parkour.Action.FreeHang"))
	{
		Play_Parkour_Montage(Free_Hang_Jump_To_Climb);
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(98.f);
	}

	else if(Parkour_Action == FGameplayTag::RequestGameplayTag("Parkour.Action.Braced.Climb.Falling.Climb"))
	{
		Play_Parkour_Montage(Braced_Jump_To_Climb_Airborne);
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(48.f);
	}
	
	else if(Parkour_Action == FGameplayTag::RequestGameplayTag("Parkour.Action.FreeHang.Falling.Climb"))
	{
		Play_Parkour_Montage(Free_Hang_Jump_To_Climb_Airborne);
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(98.f);
	}
}

float UCustom_Movement_Component::Select_Value_Based_On_Climb_Style(const FGameplayTag& Climb_Style, const float& Braced_Value, const float& Free_Hang_Value) const
{
	const float& Parkour_Braced_Value{Braced_Value};
	const float& Parkour_Free_Hang_Value{Free_Hang_Value};

	if((Climb_Style == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Climb.Style.Braced.Climb")))))
	{
		return Parkour_Braced_Value;
	}

	else //if(Climb_Style == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Climb.Style.FreeHang"))))
	{
		return Parkour_Free_Hang_Value;
	}
}

 void UCustom_Movement_Component::Measure_Wall()
 {
	//This function is used to calculate the Wall Height, Wall Depth and the Vault Height.
	//The wall height is calculated by getting the delta between the "Wall_Top_Result.Z" and the
	//characters root bone Z location.
	//The wall depth is calculated by getting the delta between the distance of 
	//"Wall_Top_Result" and "Wall_Depth_Result".
	//The vault height is calculated by getting the delta between "Wall_Depth_Result.Z" and
	//"Wall_Vault_Result.Z"

	/*If the first if statement returns false all of the values ("Wall_Height", "Wall_Depth" and "Vault_Height")
	will be set to 0. This is because if these two FHitResults don't return a blocking hit, none of 
	the other FHitResults which are needed to calculate "Wall_Height", "Wall_Depth" and "Vault_Height"
	will be valid. all of the other FHitResults which are needed to calculate "Wall_Height", "Wall_Depth" and "Vault_Height" 
	are dependant on the impact points of the FHitResults being checked in in the first if statement 
	("Initial_Ground_Level_Front_Wall_Hit_Result" and "Wall_Top_Result")*/

	//All of the following checks are to ensure that the respective FHitResults which will be used 
	//do indeed have a blocing hit. If there is no blocking hit the respective value that is being calculated, 
	//whether it be "Wall_Height", "Wall_Depth" or "Vault_Height" will be set to 0.
	
	if(Initial_Ground_Level_Front_Wall_Hit_Result.bBlockingHit && Wall_Top_Result.bBlockingHit)
	{
		Calculate_Wall_Height();

		Calculate_Wall_Depth();

		Calculate_Vault_Height();
	}

	else
	{
		Wall_Height = 0.f;
		Debug::Print(FString("Wall_Height: ") + FString::SanitizeFloat(Wall_Height), FColor::MakeRandomColor(), 1);

		Wall_Depth = 0.f;
		Debug::Print(FString("Wall_Depth: ") + FString::SanitizeFloat(Wall_Depth), FColor::MakeRandomColor(), 1);

		Vault_Height = 0.f;
		Debug::Print(FString("Vault_Height: ") + FString::SanitizeFloat(Vault_Height), FColor::MakeRandomColor(), 1);
	}
 }

void UCustom_Movement_Component::Decide_Parkour_Action()
{
	if(Wall_Top_Result.bBlockingHit)
	{
		if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))))
		{
			Debug::Print("Climb_Or_Hop", FColor::MakeRandomColor(), 7);	
		}

		else if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Free.Roam"))))
		{
			if(bIs_On_Ground)
			{
				if(Wall_Height >= 90 && Wall_Height <= 170)
				{
					if(Wall_Depth >= 0 && Wall_Depth <= 120)
					{
						if(Vault_Height >= 70 && Vault_Height <= 120)
						{
							if(Wall_Depth >= 20 && Wall_Depth <= 30)
							{
								if(UpdatedComponent->GetComponentVelocity().Size() > 20)
								{
									Debug::Print("Parkour_Low_Vault", FColor::MakeRandomColor(), 7);
								}

								else
								{
									Debug::Print("Parkour_Thin_Vault", FColor::MakeRandomColor(), 7);
								}
							}
						
							else if(UpdatedComponent->GetComponentVelocity().Size() > 20)
							{
								Debug::Print("Parkour_Low_Vault", FColor::MakeRandomColor(), 7);
							}

							else
							{
								Debug::Print("Parkour_Mantle", FColor::MakeRandomColor(), 7);
							}

						}

						else if(Vault_Height >= 130 && Vault_Height <= 140)
						{
							if(UpdatedComponent->GetComponentVelocity().Size() > 20)
							{
								Debug::Print("Parkour_High_Vault", FColor::MakeRandomColor(), 7);
							}

							else
							{
								Debug::Print("Parkour_No_Action", FColor::MakeRandomColor(), 7);
								Set_Parkour_Action(FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.No.Action"))));
							}
						
						}

						else
						{
							Debug::Print("Parkour_No_Action", FColor::MakeRandomColor(), 7);
							Set_Parkour_Action(FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.No.Action"))));
						}
					}

					else
					{
						Debug::Print("Parkour_Mantle", FColor::MakeRandomColor(), 7);
					}
				}

				else if(Wall_Height < 280)
				{
					
					Debug::Print("Parkour_Climb", FColor::MakeRandomColor(), 7);
					Decide_Climb_Style(Wall_Top_Result.ImpactPoint, Reversed_Front_Wall_Normal_Z);
					
					/*The FHitResult stored in the global FHitResult variable "Wall_Top_Result" is copied to the the global 
					FHitResult variable New_Climb_Hit_Result so that when the function "Reset_Parkour_Variables()" is called
					after each Parkour Action is complete there will still be a location to begin the next sequence of ray casts.
					*/
					Initialize_Parkour_IK_Limbs_Hit_Result = Wall_Top_Result;
					
					if(Parkour_Climb_Style == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Climb.Style.Braced.Climb"))))
					{
						Set_Parkour_Action(FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.Braced.Climb"))));
					}
					
					else
					{
						Set_Parkour_Action(FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.FreeHang"))));
					}
				}

				else
				{
					Debug::Print("No_Action", FColor::MakeRandomColor(), 7);
					Set_Parkour_Action(FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.No.Action"))));
				}
			}

			else
			{
				if(Wall_Height >= 140 && Wall_Height <= 280 && Validate_Can_Start_Shimmying_While_Airborne())
				{
					Debug::Print("Parkour_Airorne_Climb", FColor::MakeRandomColor(), 7);
					Decide_Climb_Style(Wall_Top_Result.ImpactPoint, Reversed_Front_Wall_Normal_Z);
					
					/*The FHitResult stored in the global FHitResult variable "Wall_Top_Result" is copied to the the global 
					FHitResult variable New_Climb_Hit_Result so that when the function "Reset_Parkour_Variables()" is called
					after each Parkour Action is complete there will still be a location to begin the next sequence of ray casts.
					*/
					Initialize_Parkour_IK_Limbs_Hit_Result = Wall_Top_Result;

					if(Parkour_Climb_Style == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Climb.Style.Braced.Climb"))))
					Set_Parkour_Action(FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.Braced.Climb.Falling.Climb"))));

					else
					Set_Parkour_Action(FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.FreeHang.Falling.Climb"))));
				}
			}
		}
	}
	
	else
	{
		Debug::Print("Parkour_No_Action", FColor::MakeRandomColor(), 1);
		Set_Parkour_Action(FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.No.Action"))));
	}
}

void UCustom_Movement_Component::Reset_Parkour_Variables()
{
	/*This function will be called every "Tick()" within the funtion "Parkour_Call_In_Tick()". The goal of this function is to reset the values stored in 
	the global FHitResults "Initial_Ground_Level_Front_Wall_Hit_Result", "Front_Wall_Top_Edge_Best_Hit", "Wall_Top_Result", "Wall_Depth_Result" and 
	"Wall_Vault_Result" as well as the double variables "Wall_Height", "Wall_Depth" and "Vault_Height". The resseting of the values stored in said global variables 
	needs to be completed every tick so that each time "Execute_Parkour_Action()" is called, there will be a new beginning to set the next "Parkour_State" and "Parkour_Action".*/

	float Reset{1.f};

	Initial_Ground_Level_Front_Wall_Hit_Result.Reset(Reset, false);

	Front_Wall_Top_Edge_Best_Hit.Reset(Reset, false);

	Wall_Top_Result.Reset(Reset, false);

	Wall_Depth_Result.Reset(Reset, false);

	Wall_Vault_Result.Reset(Reset, false);

	Wall_Height = 0;
	
	Wall_Depth = 0;

	Vault_Height = 0;
}

void UCustom_Movement_Component::Parkour_Call_In_Tick()
{
	/*This function will be called every "Tick()". The goal of this function is to check whether the character is on the ground or not using the function call "Validate_bIs_On_Ground()".
	Depending on the value set on the global bool variable "bIs_On_Ground" within said function another check will be performed to check if the value set to the gameplay tag "Parkour_Action" 
	is equal to "Parkour.Action.No.Action". If this is the case, then the character is on the ground and is not performing any parkour. Therefore a call to reset the values stored in 
	the global FHitResults "Initial_Ground_Level_Front_Wall_Hit_Result", "Front_Wall_Top_Edge_Best_Hit", "Wall_Top_Result", "Wall_Depth_Result" and "Wall_Vault_Result" as well as the 
	double variables "Wall_Height", "Wall_Depth" and "Vault_Height". will be made. The resetting of said values will happen within the function call "Reset_Parkour_Variables()".*/

	Validate_bIs_On_Ground();
	
	if(bIs_On_Ground && Parkour_Action == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.No.Action"))))
	{
		Reset_Parkour_Variables();
	}

	else if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))))
	{
		Dynamic_IK_Limbs();
	}

	else if(!bIs_Falling && /*Owning_Player_Character->bPressedJump &&*/ 
	Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Free.Roam"))))
	{
		Execute_Parkour_Action();
	}
}

 /*void UCustom_Movement_Component::Get_Parkour_Data_Asset(UParkour_Action_Data* Parkour_Data_Asset_To_Use)
 {
	//This function uses the global pointer that is passed in via the input argument (which is a UParkour_Action_Data* (child of UPrimaryDataAsset)) and gets the data stored within the 
	//data asset which is located at the address of said pointer (the pointer is a UPROPERTY so the data asset will be developed in the editor then stored within the character Blueprint 
	//class via the "Custom_Movement_Component" component). This happens by using the developed function call "Get_Parkour_Data_Asset_Information()" which located at said pointer's class. By passing in
	//the the global pointer which is passed into this function via the input argument, as the input parameter of the developed function call "Get_Parkour_Data_Asset_Information(), the information
	//stored within the array element of the data asset located at the address of the global pointer which is passed into this function can be obtained (again, the information is set in the editor 
	//via developing a Data Asset that is a child of the class of said pointer (UParkour_Action_Data)).
	
	if(Parkour_Data_Asset == nullptr) return;
	Parkour_Data_Asset->Get_Parkour_Data_Asset_Information(Parkour_Data_Asset_To_Use);
 }*/

void UCustom_Movement_Component::Play_Parkour_Montage(UParkour_Action_Data* Parkour_Data_Asset_To_Use)
{
	/*Safety check to make sure the pointer that is passed in via input argument through the function "Set_Parkour_Action()"
	is not a nullptr. Said pointer holds the address to the Data Asset Object which stores the animation montage to play, the 
	FGameplaytags "In_State" and "Out_State" as well as all the offset values which may need to be used by the MotionWarping 
	Component to offset the location and rotation of the root bone in relation to the impact point of the FHitResult "Wall_Top_Result"
	(this is the FHitResult which will mostly be used to set the location of the Parkour Actions when the character is in "Climb_State").*/
	 
	if(!Parkour_Data_Asset_To_Use) return;

	/*Obtain the FGameplaytag "Parkour_In_State" from within the object of the input argument "UParkour_Action_Data* 
	Parkour_Data_Asset_To_Use" (a Data Asset that is created within the editor which inherits from the developed class 
	"UParkour_Action_Data" (said class Inherits from "UPrimaryDataAsset")), via a getter function and use the value retrieved 
	as an input argument for the function "Set_Parkour_State()" This quickly sets the global FGameplaytag Parkour_State to the
	"In_State" which is set within the object of the input argument (within the editor). */
	Set_Parkour_State(Parkour_Data_Asset_To_Use->Get_Parkour_In_State());

	/*Use the "Motion_Warping_Component*" to call the function "AddOrUpdateWarpTargetFromLocationAndRotation()". 
	Said Motion Warping function is called three times because the objects of the input argument "UParkour_Action_Data* 
	Parkour_Data_Asset_To_Use may have up to three sections which may be filled with location values which may need to be used 
	to offset the location of the root bone. If the locations are left blank within said Data Asset objects, then a value of 0 
	will be passed into the input argument of the function "AddOrUpdateWarpTargetFromLocationAndRotation()" and in result the 
	location of root bone won't modified. One of the input parameters for said function is "FVector TargetLocation". For this 
	input argument, a function named "Find_Parkour_Warp_Location()" will be developed to calculate the location to offset the 
	root bone based on the location of the global FHitResult "Wall_Top_Result".*/
	Motion_Warping_Component->AddOrUpdateWarpTargetFromLocationAndRotation(
		FName(Parkour_Data_Asset_To_Use->Get_Parkour_Warp_Target_Name_1()),
		Find_Parkour_Warp_Location(
			Wall_Top_Result.ImpactPoint, 
			Reversed_Front_Wall_Normal_Z, 
			Parkour_Data_Asset_To_Use->Get_Parkour_Warp_1_X_Offset(),
			Parkour_Data_Asset_To_Use->Get_Parkour_Warp_1_Z_Offset()),
		Reversed_Front_Wall_Normal_Z);

	Motion_Warping_Component->AddOrUpdateWarpTargetFromLocationAndRotation(
		FName(Parkour_Data_Asset_To_Use->Get_Parkour_Warp_Target_Name_2()),
		Find_Parkour_Warp_Location(
			Wall_Top_Result.ImpactPoint, 
			Reversed_Front_Wall_Normal_Z, 
			Parkour_Data_Asset_To_Use->Get_Parkour_Warp_2_X_Offset(),
			Parkour_Data_Asset_To_Use->Get_Parkour_Warp_2_Z_Offset()),
		Reversed_Front_Wall_Normal_Z);

	Motion_Warping_Component->AddOrUpdateWarpTargetFromLocationAndRotation(
		FName(Parkour_Data_Asset_To_Use->Get_Parkour_Warp_Target_Name_3()),
		Find_Parkour_Warp_Location(
			Wall_Top_Result.ImpactPoint, 
			Reversed_Front_Wall_Normal_Z, 
			Parkour_Data_Asset_To_Use->Get_Parkour_Warp_3_X_Offset(),
			Parkour_Data_Asset_To_Use->Get_Parkour_Warp_3_Z_Offset()),
		Reversed_Front_Wall_Normal_Z);


	/*After the location to offset the root bone is set, use the "Anim_Instance*" to call the function "Montage_Play(). The input argument 
	"UParkour_Action_Data* Parkour_Data_Asset_To_Use" will be used to call the function "Get_Montage_To_Play()" which is a getter function that returns the 
	Animation Montage that is stored within the Data Asset object of said input argument.*/
	Anim_Instance->Montage_Play(Parkour_Data_Asset_To_Use->Get_Montage_To_Play());

	/*Next, another global pointer of the same type as the input argument "UParkour_Action_Data* Parkour_Data_Asset_To_Use" will be declared 
	"Parkour_Data_Asset", followed by being set to equal same address as the the input argument "UParkour_Action_Data* Parkour_Data_Asset_To_Use". 
	This is because the address of the object that is passed into this function needs to be stored globally so that said address may be passed into 
	the function "Parkour_State()" as an input argument that obtains the FGameplayTag "Out_State" from the Data Asset object which is being used in this 
	function. "Parkour_State()" will be called from within the function "Function_To_Execute_On_Animation_Blending_Out()". "Function_To_Execute_On_Animation_Blending_Out()" 
	will be called when the developed local "FOnMontageEnded Blending_Out_Delegate" is called (when Parkour_Data_Asset_To_Use->Get_Montage_To_Play() is blending out).*/
	Parkour_Data_Asset = Parkour_Data_Asset_To_Use;

	/*Develop a delegate which will call the function "Function_To_Execute_On_Animation_Blending_Out()"" when the animation montage is blending out.
	Within said function the global "FGameplayTag "Parkour_State" will be updated with the FGameplayTag "Out_State" which is set in the object of the 
	input argument to this function "UParkour_Action_Data* Parkour_Data_Asset_To_Use". The address to this object is copied to the global 
	"UParkour_Action_Data* Parkour_Data_Asset". Therefore global "UParkour_Action_Data* Parkour_Data_Asset" is used to obtain the FGameplayTag "Out_State"
	from within the Data Asset Object which is being used in this function.*/
	FOnMontageEnded Blending_Out_Delegate{};
	Blending_Out_Delegate.BindUObject(this, &UCustom_Movement_Component::Function_To_Execute_On_Animation_Blending_Out);
	Anim_Instance->Montage_SetBlendingOutDelegate(Blending_Out_Delegate, Parkour_Data_Asset_To_Use->Get_Montage_To_Play());
	
	/*When this function is done with everything else, set the Parkour Action to "Parkour.Action.No.Action" so that 
	Reset_Parkour_Variables() can be called to reset the values stored in the global FHitResults "Initial_Ground_Level_Front_Wall_Hit_Result", 
	"Front_Wall_Top_Edge_Best_Hit", "Wall_Top_Result", "Wall_Depth_Result" and "Wall_Vault_Result" as well as the double variables "Wall_Height", 
	"Wall_Depth" and "Vault_Height".*/
	Set_Parkour_Action(FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.No.Action"))));
}

FVector UCustom_Movement_Component::Find_Parkour_Warp_Location(const FVector& Impact_Point_To_Use, const FRotator& Direction_For_Character_To_Face, const float& X_Axis_Offset, const float& Z_Axis_Offset) const
{
	/*The goal of this function is to take the input parameters "Impact_Point_To_Use", "Direction_For_Character_To_Face", 
	"X_Axis_Offset" and "Z_Axis_Offset" and move the vector forward or backwards using the helper function "Move_Vector_Forward", 
	followed by moving the result of the previous helper function "Move_Vector_Forward" up or down by using the helper function 
	"Move_Vector_Up". The result of the latter will be returned and this will be the location for the Motion Warping Component 
	to place the root bone for the respective Motion Warping anim notify. The input arguments for this function will be filled 
	in with the values from the Data Asset pointer that is passed into the function "Play_Parkour_Montage()".*/

	const FVector Warp_Location_First_Edit{Move_Vector_Forward(Impact_Point_To_Use, Direction_For_Character_To_Face, X_Axis_Offset)};
	const FVector Warp_Location_Second_Edit{Move_Vector_Up(Warp_Location_First_Edit, Z_Axis_Offset)};

	const FVector Destination{Warp_Location_Second_Edit};

	return Destination;
}

void UCustom_Movement_Component::Function_To_Execute_On_Animation_Blending_Out(UAnimMontage *Montage, bool bInterrupted)
{
	/*This function will be called from within the function "Play_Parkour_Montage()" to set the global FGameplayTag "Parkour_State" to equal the FGameplayTag "Out_State"
	found within the Data_Asset object which is being used in the function "Play_Parkour_Montage" (Data_Asset object is passed in via the input argument of said function). 
	The "UParkour_Action_Data* Parkour_Data_Asset" is initialized with the address of the Data_Asset which is being used in the function "Play_Parkour_Montage()". Said 
	initialization also happens within the function "Play_Parkour_Montage()" Lastly this function serves as a response to an delegate call back which triggers when the montage 
	in the function "Play_Parkour_Montage()" is blending out.*/ 
	Set_Parkour_State(Parkour_Data_Asset->Get_Parkour_Out_State());
	return Debug::Print(TEXT("Animation Blended out and Parkour_State set from Data Asset used."));
}

void UCustom_Movement_Component::Add_Movement_Input(const FVector2D& Scale_Value, const bool& bIs_Forward_Backward_Movement)
{
	//This function is called within the character class in "&ATechnical_Animator_Character::Handle_Ground_Movement_Input_Triggered". It handles the ground locomotion of the 
	//character when the FGameplaytag for the "Parkour_State" is set to "Parkour.State.Free.Roam" as well as the locomotion of the character when the FGameplayTag for the 
	//"Parkour_State" is set to "Parkour.State.Climb".
	
	//This boolean variable is set to true or false within the call to this function within the character blueprint. When it is set to true, the global double variable "Forward_Backward_Movement_Value"
	//is set to be the same value as the the input parameter variable "Scale_Value.Y" when it is set to false the global double variable "Right_Left_Movement_Value" is set to
	//the same value as the input parameter variable "Scale_Value.X". These two variables determine whether the character is moving forward or backward (Forward_Backward_Movement_Value) or left to right (Right_Left_Movement_Value).
	if(bIs_Forward_Backward_Movement)
	Forward_Backward_Movement_Value = Scale_Value.Y;

	else
	Right_Left_Movement_Value = Scale_Value.X;

	//checking to see the current "Parkour_State" of the character. If the value equals "Parkour.State.Free.Roam" then this means the character should have it's normal ground locomotion.
	//If the "Parkour_State" is set to "Parkour.State.Climb" then a call to handle "Parkour_Climb_Movement()" should be made.
	if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Free.Roam"))))
	{
		//This variable is set within the character class in one of the calls to this function. One call sets this variable to true the other sets this variable to false. In each respective call to this function the global double variables
		//Forward_Backward_Movement_Value" and "Right_Left_Movement_Value" is set with the correct "Y" and "X" values from the "FInputAction Value"from the character class so the gound locomotion may work as expected. (The "FInputAction Value" is converted to
		//an FVector2D within the character class using "Value.Get<FVector2D>()").
		if(bIs_Forward_Backward_Movement)
		{	
			/*Get the forward vector from the controller by accessing the controller rotation Yaw and getting it's forward vecor. Use said forward vector to "AddMovementInput" to the character using the "Forward_Backward_Movement_Value"*/
			
			//Store the controller rotation in a variable.
			const FRotator& Controller_Rotation{Owning_Player_Character->GetControlRotation()};
			
			//Subtract the Pitch and the Roll from the controller rotation and store the Yaw into a variable.
			const FRotator& Controller_Rotation_Modified{FRotator(Controller_Rotation.Pitch - Controller_Rotation.Pitch, Controller_Rotation.Yaw, Controller_Rotation.Roll - Controller_Rotation.Roll)};
			
			//Store the forward vector of the controller Yaw into a variable.
			const FVector& Forward_Direction{UKismetMathLibrary::GetForwardVector(Controller_Rotation_Modified)};

			//Add movement input to the character using the forward vector of the controller along with the "Forward_Backward_Movement_Value".
			Owning_Player_Character->AddMovementInput(Forward_Direction, Forward_Backward_Movement_Value);
			
			Get_Controller_Direction();
		}

		else
		{
			/*Get the right vector from the controller by accessing the controller rotation Yaw and Roll. Use said right vector to "AddMovementInput" to the character using the "Right_Left_Movement_Value"*/

			//Store the controller rotation in a variable.
			const FRotator& Controller_Rotation{Owning_Player_Character->GetControlRotation()};

			//Subtract the Pitch from the controller rotation and store the Yaw and Roll into a variable.
			const FRotator& Controller_Rotation_Modified{FRotator (Controller_Rotation.Pitch - Controller_Rotation.Pitch, Controller_Rotation.Yaw, Controller_Rotation.Roll)};
			
			//Store the right vector of the controller Yaw and Roll into a variable.
			const FVector& Right_Direction{UKismetMathLibrary::GetRightVector(Controller_Rotation_Modified)};

			//Add movement input to the character using the right vector of the controller along with the "Right_Left_Movement_Value".
			Owning_Player_Character->AddMovementInput(Right_Direction, Right_Left_Movement_Value);

			Get_Controller_Direction();
		}
	}

	//If the Parkour_State is set to "Parkour.State.Climb" check to see if there is an animation playing. If so call "Stop_Parkour_Climb_Movement_Immediately_And_Reset_Movement_Input_Variables()". This call will ensure that the character clamps to the surface
	//of the wall when transitioning the global FGameplayTag "Parkour_State" from "Parkour.State.Free.Roam" to "Parkour.State.Climb" as well as when other montages as played when the global FGameplayTag "Parkour_State" is set to
	//"Parkour.State.Climb". If no animation is playing call the function "Parkour_Climb_Movement()". This function handles all the logic for the climb movement.
	else if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))))
	{
		if(Anim_Instance->IsAnyMontagePlaying())
		{
			Stop_Parkour_Climb_Movement_Immediately_And_Reset_Movement_Input_Variables();
		}
		
		else
		{
			Parkour_Climb_Movement();
		}
	}
}

void UCustom_Movement_Component::Stop_Parkour_Climb_Movement_Immediately_And_Reset_Movement_Input_Variables()
{
	//This function is called within the character class in "&ATechnical_Animator_Character::Handle_Ground_Movement_Input_Triggered" as 
	//well as within this class. It resets the values of the global double variables named "Forward_Backward_Movement_Value" and 
	//"Right_Left_Movement_Value" (both set within the "Add_Movement_Input" function). 
	//In this function the Parkour Direction is also set to "Parkour.Direction.None".

	//This function also stops the movement of the character immediately.
	Character_Movement->StopMovementImmediately();
	
	Set_Parkour_Direction(FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.None"))));

	Forward_Backward_Movement_Value = 0.f;

	Right_Left_Movement_Value = 0.f;
}

void UCustom_Movement_Component::Parkour_Climb_Movement()
{
	 
	if(Parkour_Action == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.Corner.Move"))))
	{
		Parkour_Climb_Handle_Corner_Movement();
	}

	else
	{
		Parkour_Climb_Handle_Shimmying_Movement();
	}
}

void UCustom_Movement_Component::Parkour_Climb_Handle_Shimmying_Movement()
{
	/*This function handles calling the functions which will validate whether the character can shimmy in the direction of the input which is passed into the gloabal double variable "Right_Left_Movement_Value". 
	within the function "&UCustom_Movement_Component::Add_Movement_Input".*/
	
	//Store the absolute value of the value which is passed into the global double variable "Right_Left_Movement_Value". This value will be used to check if the input to move the character to the right or left
	//within the function "&UCustom_Movement_Component::Add_Movement_Input" is above the threshold to accept input.
	const double Right_Left_Movement_Value_Absolute_Value{UKismetMathLibrary::Abs(Right_Left_Movement_Value)};

	//Check to see if the absolute value of the value which is passed into the global double variable "Right_Left_Movement_Value" is above the threshold to allow shimmying movement.
	//If the check is passed, check to see if the value is above or below 0. If the value is above 0 the character is moving to the right, if the value is below 0 the character is 
	//moving to the left.
	if(Right_Left_Movement_Value_Absolute_Value > .7)
	{
		if(Get_Controller_Direction() == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Right"))))
		{
			Set_Parkour_Direction(FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Right"))));
		}

		else if(Get_Controller_Direction() == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Left"))))
		{
			Set_Parkour_Direction(FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Left"))));
		}

		//These variables are filled with values within the function "Parkour_Climb_State_Detect_Wall" (they are passsed in as references via the input arguments in said function). 
		//The FHitResult stored within the variables "Parkour_Climbing_Detect_Wall_Hit_Result" and "Parkour_Climbing_Wall_Top_Result" are be used to determine whether there is a wall 
		//in front of the character which the character can shimmy across/ up or down via the functions "Parkour_Climb_State_Are_There_Obstacles_On_Sides_Of_Hands"  and
		//"Parkour_Climb_State_Are_There_Obstacles_On_Sides_Of_Body". Said FHitResult variables are passed in as const references via the input arguments (they are filled with data 
		//within the function "Parkour_Climb_State_Detect_Wall").
		FHitResult Parkour_Climbing_Detect_Wall_Hit_Result{};
		FHitResult Parkour_Climbing_Wall_Top_Result{};
		

		//This function executes an algorithm which determines whether there is a wall infront of the character for climb movememnt to happen, as well as determining if the wall
		//in the direction in which the character is moving is too high or low to shimmy across. For example if the character is moving to the left and the surface which the character
		//is shimmying across has a ledge that is too high for the character to reach aka bStartPenetrating is true, (calculated by the maximum number of iterations in the inner for loop), 
		//then "Parkour_Climbing_Wall_Top_Result" will not be calculated and "Stop_Parkour_Climb_Movement_Immediately_And_Reset_Movement_Input_Variables" along with "return" will be called 
		//within the function in the appropriate location. Same goes for the "Parkour_Climbing_Detect_Wall_Hit_Result". If there is no blocking hit or "bStartPenetrating" is true 
		//"Stop_Parkour_Climb_Movement_Immediately_And_Reset_Movement_Input_Variables" along with "return" will be called within the function in the appropriate location within the function.
		if(!Parkour_Climb_State_Detect_Wall(Parkour_Climbing_Detect_Wall_Hit_Result, Parkour_Climbing_Wall_Top_Result))
		{
			Stop_Parkour_Climb_Movement_Immediately_And_Reset_Movement_Input_Variables();
			return;
		}
		
		//This if check determines whether there are obstacles on the side of the character's hands which the should stop the character from shimmying any furhther is that direction.
		//The function "Parkour_Climb_State_Are_There_Obstacles_On_Sides_Of_Hands" uses "Parkour_Climbing_Wall_Top_Result.ImpactPoint" as the starting location (const reference input parameter)
		//of the line traces executed within said function.
		else if(Parkour_Climb_State_Are_There_Obstacles_On_Sides_Of_Hands(Parkour_Climbing_Wall_Top_Result.ImpactPoint))
		{
			Stop_Parkour_Climb_Movement_Immediately_And_Reset_Movement_Input_Variables();
			return;
		}

		//This check determines whether there is a obstacle on the side of the character's body which should deter the character from shimmying any further. The starting location 
		//(const reference input parameter) of the capsule trace executed within the bool function "Parkour_Climb_State_Are_There_Obstacles_On_Sides_Of_Body" is the impact point of 
		//"Parkour_Climbing_Detect_Wall_Hit_Result".
		else if(Parkour_Climb_State_Are_There_Obstacles_On_Sides_Of_Body(Parkour_Climbing_Detect_Wall_Hit_Result.ImpactPoint))
		{
			Stop_Parkour_Climb_Movement_Immediately_And_Reset_Movement_Input_Variables();
			return;
		}

		//If this line of code is reached then character has room to move and in result "Calculate_And_Move_Character_To_New_Climb_Position" should be called passing in the FHitResults
		//"Parkour_Climbing_Detect_Wall_Hit_Result" and "Parkour_Climbing_Wall_Top_Result" into the input argument. This function uses the location of said FHitResults to interpolate the
		//character to their location. Considering the locations of said FHitResults are always right infornt of the arrow (offset to the right or left side of the arrow actor depending
		//on whether the character is moving to the right or left side) the character will always be "chasing"	the location to interpolate its location to causeing an infinite interpolation
		//to the location of the FHitResults and in return "Shimmying_Movement" as long as ther is input into the controller.
		else
		{
			Calculate_And_Move_Character_To_New_Climb_Position(Parkour_Climbing_Detect_Wall_Hit_Result, Parkour_Climbing_Wall_Top_Result);
		}
	}

	//If "Right_Left_Movement_Value_Absolute_Value" i not above .7 then this means the minimum threshold to activate "Shimmying_Movement" has not been met by the input from the player's
	//controller. In this case, "Stop_Parkour_Climb_Movement_Immediately_And_Reset_Movement_Input_Variables" and "return" should be called.
	else
	{
		Stop_Parkour_Climb_Movement_Immediately_And_Reset_Movement_Input_Variables();
		return;
	}
}

void UCustom_Movement_Component::Calculate_And_Move_Character_To_New_Climb_Position(const FHitResult& Parkour_Climbing_Detect_Wall_Hit_Result, const FHitResult& Parkour_Climbing_Wall_Top_Result)
{
	/*This function is called calculates the location to interpolate the character to and passes in the said location into the function "&UCustom_Movement_Component::Move_Character_To_New_Climb_Position_Interpolation_Settings"
	as an input argument, well as calling the function which decides the Climb_Style which the character should be in depending on the surface of the wall in which the character is shimmying. */
	
	//Offset value to be used to offset the character backwards from the wall. This is because the impact points found within the input parameter "Parkour_Climbing_Detect_Wall_Hit_Result" is right on
	//the surface of the wall. Therefore the character needs to be moved back so that the animation playing will look realistic and natural. 
	const float& Offset_Character_Backwards_From_Wall_Value{Select_Value_Based_On_Climb_Style(Parkour_Climb_Style, 40.f, 7.f)};
	const FVector Offset_Vector_Backwards_From_Wall{Move_Vector_Backward(Parkour_Climbing_Detect_Wall_Hit_Result.ImpactPoint, Reversed_Front_Wall_Normal_Z, Offset_Character_Backwards_From_Wall_Value)};



	//According to the "Parkour_Climb_Style" this is the value to offset the character in the "Z" axis from the location of the "Parkour_Climbing_Wall_Top_Result.ImpactPoint.Z". 
	const float& Pick_Climb_Style_Value_Character_Height{Select_Value_Based_On_Climb_Style(Parkour_Climb_Style, 55.f, 103.f)};
	
	

	/*These values are used to make a custom FVector variable ("Move_Character_To_This_Location").*/

	//Value to use on the "X" axis of the custom FVector "Move_Character_To_This_Location".	
	const double& Set_Character_To_This_Position_Perpendicular_From_Wall{Offset_Vector_Backwards_From_Wall.X};			
	//Value to use on the "Y" axis of the custom FVector "Move_Character_To_This_Location".	
	const double& Set_Character_To_This_Position_Parallel_From_Wall{Offset_Vector_Backwards_From_Wall.Y};
	//Value to use on the "Z" axis of the custom FVector "Move_Character_To_This_Location".	
	const double& Set_Character_To_This_Height_Position{Parkour_Climbing_Wall_Top_Result.ImpactPoint.Z - Pick_Climb_Style_Value_Character_Height};


	//Custom FVector to pass into the function "&UCustom_Movement_Component::Move_Character_To_New_Climb_Position_Interpolation_Settings" as an input argument. This will be the location to interpolate 
	//the character to as long as there is input into the player controller and the validation and checks performed in the function "&UCustom_Movement_Component::Parkour_Climb_Handle_Shimmying_Movement"
	//are successful.. 
	const FVector& Move_Character_To_This_Location(FVector(Set_Character_To_This_Position_Perpendicular_From_Wall, 
														   Set_Character_To_This_Position_Parallel_From_Wall, 
														   Set_Character_To_This_Height_Position));

	//Call the function "&UCustom_Movement_Component::Decide_Climb_Style" to determine which "Parkour_Climb_Style" to set the character to. This function uses an offset location units below the 
	//Wall Top Result passed into it to generate ray casts at the level of the characters feet when said character is in the braced climb style. If there is no blocking hit on the ray cast
	//then the characters "Parkour_Climb_Style" will be set to "Parkour.Climb.Style.FreeHang".
	Decide_Climb_Style(Parkour_Climbing_Wall_Top_Result.ImpactPoint, Reversed_Front_Wall_Normal_Z);

	//This function uses the location which is calculated above (using the variable from the FHitResult input argument "Parkour_Climbing_Detect_Wall_Hit_Result") to interpolate the character to said 
	//FVector. Considering the locations of the custom FVector will always be updating due to it being dependant on the input argument variable "Parkour_Climbing_Detect_Wall_Hit_Result",  
	//the character will always be "chasing" the location to interpolate its location to causeing an infinite interpolation. This is because the impact point of the input argument 
	//"Parkour_Climbing_Detect_Wall_Hit_Result" is offset to the right or left side of the arrow actor (the arrow actor is just above the character) depending on whether the character is moving
	//to the right or left.
	Move_Character_To_New_Climb_Position_Interpolation_Settings(Move_Character_To_This_Location, Reversed_Front_Wall_Normal_Z);

	//const FVector Offset_Decide_Climb_Style_Impact_Point{Move_Vector_Right(Parkour_Climbing_Wall_Top_Result.ImpactPoint, Reversed_Front_Wall_Normal_Z, Right_Left_Movement_Value * -10.f)};
	//Decide_Climb_Style(Offset_Decide_Climb_Style_Impact_Point, Reversed_Front_Wall_Normal_Z);
}

void UCustom_Movement_Component::Move_Character_To_New_Climb_Position_Interpolation_Settings(const FVector& Location_To_Move_Character, const FRotator& Rotation_For_Character_To_Face)
{
	/*Using the location from the input argument "Location_To_Move_Character" which is is calculated in the function "&UCustom_Movement_Component::Calculate_And_Move_Character_To_New_Climb_Position"
	(via said functions const reference FHitResult input argument "Parkour_Climbing_Detect_Wall_Hit_Result"), this function handles interpolating the character to said location. 
	Considering the location of the input parameter FVector "Location_To_Move_Character" will always be updating due to it being dependant on the input argument variable (of the function 
	"&UCustom_Movement_Component::Calculate_And_Move_Character_To_New_Climb_Position") "Parkour_Climbing_Detect_Wall_Hit_Result", the character will always be "chasing" the location to interpolate 
	its location to causeing an infinite interpolation. This is because the impact point of the input argument (of the function "&UCustom_Movement_Component::Calculate_And_Move_Character_To_New_Climb_Position") 
	"Parkour_Climbing_Detect_Wall_Hit_Result" is offset to the right or left side of the arrow actor (the arrow actor is just above the character) depending on whether the character is moving
	to the right or left.*/
	
	//Depending on the "Parkour_Climb_Style" the interpolation speed for the "X" and "Y" axis (speed which the character moves when shimmying forwards/backwards or from the left to right) will be selected.
	const float& Pick_Climb_Style_Value_Interpolation_Speed_For_X_And_Y_Axis{Select_Value_Based_On_Climb_Style(Parkour_Climb_Style, 5.f, 3.5f)};
	
	//Depending on the "Parkour_Climb_Style" the interpolation speed for the "Z" axis (speed which the character moves when shimmying up or down a ledge) will be selected.
	const float& Pick_Climb_Style_Value_Interpolation_Speed_For_Z_Axis{Select_Value_Based_On_Climb_Style(Parkour_Climb_Style, 2.f, 7.f)};

	/*These variables hold the data for each of the axis for which to interpolate the character. Said data is passed in via const reference input argument "Location_To_Move_Character". The reason why the data is divided into 
	seperate axes is because for each axis, different interpolation values will be set based on the "Parkour_Climb_Style"*/
	const double& Location_To_Move_Character_X{Location_To_Move_Character.X};
	const double& Location_To_Move_Character_Y{Location_To_Move_Character.Y};
	const double& Location_To_Move_Character_Z{Location_To_Move_Character.Z};

	/*These variables hold the data for the current location of the "UpdatedComponent". The reason why the data is divided into seperate axes is because for each axis, different interpolation values will be set based on 
	the "Parkour_Climb_Style"*/
	const FVector Component_Location{UpdatedComponent->GetComponentLocation()};
	const double& Component_Location_X{Component_Location.X};
	const double& Component_Location_Y{Component_Location.Y};
	const double& Component_Location_Z{Component_Location.Z};

	//"DeltaTime" is needed to fulfill the requirements of the input argument for the funtion "UKismetMathLibrary::FInterpTo". By using DeltaTime there will be a smooth interpolation from the previous location to the new location.
	const double& DeltaTime{UGameplayStatics::GetWorldDeltaSeconds(this)};

	/*These variables hold the data of the interpolation from the "UpdatedComponent's" current location to the location which the character needs to move to. The reason why the data is divided into seperate axes is because for 
	each axis, different interpolation values are set based on the "Parkour_Climb_Style".*/
	const double X_Interpolation{UKismetMathLibrary::FInterpTo(Component_Location_X, Location_To_Move_Character_X, DeltaTime, Pick_Climb_Style_Value_Interpolation_Speed_For_X_And_Y_Axis)};
	const double Y_Interpolation{UKismetMathLibrary::FInterpTo(Component_Location_Y, Location_To_Move_Character_Y, DeltaTime, Pick_Climb_Style_Value_Interpolation_Speed_For_X_And_Y_Axis)};
	const double Z_Interpolation{UKismetMathLibrary::FInterpTo(Component_Location_Z, Location_To_Move_Character_Z, DeltaTime, Pick_Climb_Style_Value_Interpolation_Speed_For_Z_Axis)};

	//This variable holds the interpolated data of where the caracter needs to move to.
	const FVector& Interpolated_Location_To_Move_Character{FVector(X_Interpolation, Y_Interpolation, Z_Interpolation)};

	//Call "SetActorLocationAndRotation" and pass in the variable which holds the interpolated data where the caracter needs to move to (Interpolated_Location_To_Move_Character).
	Owning_Player_Character->SetActorLocationAndRotation(Interpolated_Location_To_Move_Character, Rotation_For_Character_To_Face);

	return;
}

void UCustom_Movement_Component::Dynamic_IK_Limbs()
{
	if(Parkour_State != FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))))
	return;

	Parkour_Climb_Dynamic_IK_Hands(true);
	Parkour_Climb_Dynamic_IK_Hands(false);
	Parkour_Climb_Dynamic_IK_Feet(true);
	Parkour_Climb_Dynamic_IK_Feet(false);
}

void UCustom_Movement_Component::Reset_Parkour_IK_Hands(bool bIs_Left_Hand)
{
	if(Parkour_State != FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))))
	return;

	FVector IK_Hand_Location{};
	FRotator IK_Hand_Rotation{};

	if(bIs_Left_Hand == true)
	{
		IK_Hand_Location = Mesh->GetSocketLocation(FName(TEXT("ik_hand_l")));
		Parkour_Interface->Execute_Set_Left_Hand_Shimmy_Location(Anim_Instance, IK_Hand_Location);

		IK_Hand_Rotation = Mesh->GetSocketRotation(FName(TEXT("ik_hand_l")));
		Parkour_Interface->Execute_Set_Left_Hand_Shimmy_Rotation(Anim_Instance, IK_Hand_Rotation);
	}

	else
	{
		IK_Hand_Location = Mesh->GetSocketLocation(FName(TEXT("ik_hand_r")));
		Parkour_Interface->Execute_Set_Right_Hand_Shimmy_Location(Anim_Instance, IK_Hand_Location);

		IK_Hand_Rotation = Mesh->GetSocketRotation(FName(TEXT("ik_hand_r")));
		Parkour_Interface->Execute_Set_Right_Hand_Shimmy_Rotation(Anim_Instance, IK_Hand_Rotation);
	}
}

void UCustom_Movement_Component::Reset_Parkour_IK_Feet(bool bIs_Left_Hand)
{
	if(Parkour_State != FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))))
	return;

	FVector IK_Foot_Location{};
	FRotator IK_Foot_Rotation{};

	if(bIs_Left_Hand == true)
	{
		IK_Foot_Location = Mesh->GetSocketLocation(FName(TEXT("ik_foot_l")));
		Parkour_Interface->Execute_Set_Left_Foot_Shimmy_Location(Anim_Instance, IK_Foot_Location);

		IK_Foot_Rotation = Mesh->GetSocketRotation(FName(TEXT("ik_foot_l")));
		Parkour_Interface->Execute_Set_Left_Foot_Shimmy_Rotation(Anim_Instance, IK_Foot_Rotation);
	}

	else
	{
		IK_Foot_Location = Mesh->GetSocketLocation(FName(TEXT("ik_foot_r")));
		Parkour_Interface->Execute_Set_Right_Foot_Shimmy_Location(Anim_Instance, IK_Foot_Location);

		IK_Foot_Rotation = Mesh->GetSocketRotation(FName(TEXT("ik_foot_r")));
		Parkour_Interface->Execute_Set_Right_Foot_Shimmy_Rotation(Anim_Instance, IK_Foot_Rotation);
	}
}

void UCustom_Movement_Component::Release_From_Shimmying()
{
	/*This function is called within the character class &ATechnical_Animator_Character::On_Parkour_Ended.*/
	
	//This function should only be called when the character is shimmying. Therefore check to see if the character's "Parkour_State" is set to FGameplayTag "Parkour.State.Climb".
	if(Parkour_State != FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))))
	return;

	//This function will handle releasing the character from shimmying on the current surface. If this is the case "bIs_On_Ground" should be false because when shimmying the character should be above the ground
	//and in result the box trace which is constantly being executed at the location of the root bone should not return a hit result (with no hit result the global bool variable "bIs_On_Ground" is set to false). 
	if(bIs_On_Ground)
	return;

	//"&UCustom_Movement_Component::Handle_Release_From_Shimmying" will handle which animation plays when the call to release the character from the current "Climb_State" is called from the character class 
	//(in "&ATechnical_Animator_Character::On_Parkour_Ended")
	Handle_Release_From_Shimmying();

	//Set the capsule component back to normal size. During shimmying the size may change according to the "Parkour_Climb_Style"
	Owning_Player_Character->GetCapsuleComponent()->SetCapsuleHalfHeight(98.f);

	//Set the global bool variable "bIs_Falling" to true. When this variable is true, no other Parkour action will be able to be executed.
	bIs_Falling = true;
	Debug::Print("bIs_Falling Set To True");

	//Set a timer to call the function "&UCustom_Movement_Component::Set_bIs_Falling_To_False" which will set the global variable "bIs_Falling" back to false so that Parkour may be re-enabled.
	if(Owning_Player_Character)
	{
		Owning_Player_Character->GetWorldTimerManager().SetTimer(
			Set_bIs_Falling_To_False_Timer_Handle,
			this,
			&UCustom_Movement_Component::Set_bIs_Falling_To_False,
			Set_bIs_Falling_To_False_Timer_Duration
		);
	}
}

void UCustom_Movement_Component::Handle_Release_From_Shimmying()
{
	/*This function is called within &UCustom_Movement_Component::Release_From_Shimmying and it handles what happens when the
	character wants to be stop shimmying. Depending on the input placed into the character controller via the global double variables
	"Forward_Backward_Movement_Value" and "Right_Left_Movement_Value" and the current "Parkour_Climb_Style" set on the character, a call to execute
	the appropriate montage (which is stored within the object of the global pointer type UParkour_Action_Data* in the form of a Data Asset) will 
	be made via &UCustom_Movement_Component::Play_Parkour_Montage*/

	//Set the "Parkour_State" to "Parkour.State.Free.Roam". When this happens the collisions on the character's capsule component are enabled and the character's movement mode is set to "EMovementMode::Walking". Also,
	//"Character_Movement->StopMovementImmediately" is set to false (this is set to true when the character is shimmying).
	Set_Parkour_State(FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Free.Roam"))));

	//Store the absolute value of the value which is passed into the global double variable "Right_Left_Movement_Value". This value will be used to check if the input to move the character to the right or left
	//within the function "&UCustom_Movement_Component::Add_Movement_Input" is above the threshold to accept input.
	const double Forward_Backward_Movement_Value_Absolute_Value{UKismetMathLibrary::Abs(Forward_Backward_Movement_Value)};
	
	//Check to see if the character's "Climb_Style" is currently set to "Parkour.Climb.Style.Braced.Climb" and there is current input in the player controller to move the character either forwards or backwards. 
	//If so check to see if there is input fromt the player controller to either move the character to the left or the right. Depending on what value is stored in the global double variable "Right_Left_Movement_Value"
	//from the player controller the animation to dismantle the character from "FGameplayTag "Parkour.Climb.Style.Braced.Climb" will play. There are only rotation animations for "Parkour.Climb.Style.Braced.Climb".
	if(Parkour_Climb_Style == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Climb.Style.Braced.Climb"))) && Forward_Backward_Movement_Value_Absolute_Value > 0.f)
	{
		if(Get_Controller_Direction() == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Backward.Right"))))
		Play_Parkour_Montage(Ledge_Fall_Down_180_R);

		else if(Get_Controller_Direction() == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Backward.Left"))))
		Play_Parkour_Montage(Ledge_Fall_Down_180_L);
	}

	//If the current "Climb_State" is currently not "Parkour.Climb.Style.Braced.Climb" && there is no input from the player controller the animation will play to release the character from the current "Climb_Style".
	//Said animation will leave the character facing the same direction as when shimmying was enabled. 
	else
	{
		if(Parkour_Climb_Style == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Climb.Style.Braced.Climb"))))
		Play_Parkour_Montage(Ledge_Fall_Down);

		else if(Parkour_Climb_Style == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Climb.Style.Braced.Climb"))))
		Play_Parkour_Montage(Hanging_Drop);
	}
}

void UCustom_Movement_Component::Set_bIs_Falling_To_False()
{
	/*This function is called within &UCustom_Movement_Component::Release_From_Shimmying.*/
	
	//When the the global bool variable "bIs_Falling" is set to true within &UCustom_Movement_Component::Release_From_Shimmying a cool down timer
	//is started. Until this cool down timer is complete the character will not be able to call the function &UCustom_Movement_Component::Execute_Parkour_Action.
	bIs_Falling = false;
	Debug::Print("bIs_Falling Set To False");
}

bool UCustom_Movement_Component::Validate_Can_Start_Shimmying_While_Airborne() const
{
	/*This function is called wihtin &UCustom_Movement_Component::Decide_Parkour_Action. It determines whether the character can start shimmying  when airborne.
	This is done by getting the location of the global variable "Wall_Top_Result.ImpactPoint" and the  "head" socket location followed by calculating the delta between the two locations. If the 
	character's head is high enough above the gloabal variable "Wall_Top_Result.Impactpoint" "true" is returned otherwise "false" is returned.*/

	//Check to see if the global variable "Wall_Top_Result" has a blocking hit. If not return out of this function. The blocking hit stored in this variable is used
	//to get the global height of the surface which the character will start shimmying if the condition is met.
	if(!Wall_Top_Result.bBlockingHit)
	return false;

	const double Wall_Top_Result_Global_Height{Wall_Top_Result.ImpactPoint.Z};

	//Get the global height of the character's head. This value will be used to get the delta between itself and the height stored in the global variable 
	//"Wall_Top_Result.Z".
	double Characters_Head_Global_Height{};

	if(Mesh)
	Characters_Head_Global_Height = Mesh->GetSocketLocation(FName(TEXT("head"))).Z;

	//Get the delta value between the global height of the character's head socket and the global height of the surface which is being analyzed for shimmying.
	const double Delta_Between_Wall_Top_Result_Global_Height_And_Characters_Head_Global_Height{Wall_Top_Result_Global_Height - Characters_Head_Global_Height};

	//Check to see if the delta between the two locations (Characters_Head_Global_Height && Wall_Top_Result_Global_Height) is greater than 30. If this is true 
	//The character is high enough to start shimmying while airborne. Return true in this case. If this value is not met return false.
	const double Select_Threshold_Value_Based_On_Climb_Style{Select_Value_Based_On_Climb_Style(Parkour_Climb_Style, 30.f, 35.f)};
	
	if(Delta_Between_Wall_Top_Result_Global_Height_And_Characters_Head_Global_Height <= Select_Threshold_Value_Based_On_Climb_Style)
	{
		Debug::Print("Delta_Between_Wall_Top_Result_Global_Height_And_Characters_Head_Global_Height: " + FString::FromInt(Delta_Between_Wall_Top_Result_Global_Height_And_Characters_Head_Global_Height), FColor::Green, 10);
		return true;
	}
	
	else
	return false;

	//The following line of code is used to for extra precaution.
	return false;
}

FGameplayTag UCustom_Movement_Component::Get_Controller_Direction() const
{
	/*This is similar to the helper functions. In this function the value stored in the two double variables
	"Forward_Backward_Movement_Value" and "Right_Left_Movement_Value" are analyzed. Depending on the result 
	an FGameplayTag which represents the directional input of the controller is returned*/

	if(Forward_Backward_Movement_Value > 0.f && Right_Left_Movement_Value == 0.f)
	{
		Debug::Print("Parkour.Direction.Forward", FColor::Purple, 1);
		return FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Forward")));
	}

	else if(Forward_Backward_Movement_Value < 0.f && Right_Left_Movement_Value == 0.f)
	{
		Debug::Print("Parkour.Direction.Backward", FColor::Purple, 1);
		return FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Backward")));
	}

	else if(Forward_Backward_Movement_Value == 0.f && Right_Left_Movement_Value < 0.f)
	{
		Debug::Print("Parkour_Direction_Left", FColor::Purple, 1);
		return FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Left")));
	}

	else if(Forward_Backward_Movement_Value == 0.f && Right_Left_Movement_Value > 0.f)
	{
		Debug::Print("Parkour_Direction_Right", FColor::Purple, 1);
		return FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Right")));
	}

	else if(Forward_Backward_Movement_Value > 0.f && Right_Left_Movement_Value < 0.f)
	{
		Debug::Print("Parkour.Direction.Forward.Left", FColor::Purple, 1);
		return FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Forward.Left")));
	}

	else if(Forward_Backward_Movement_Value > 0.f && Right_Left_Movement_Value > 0.f)
	{
		Debug::Print("Parkour.Direction.Forward.Right", FColor::Purple, 1);
		return FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Forward.Right")));
	}

	else if(Forward_Backward_Movement_Value < 0.f && Right_Left_Movement_Value < 0.f)
	{
		Debug::Print("Parkour.Direction.Backward.Left", FColor::Purple, 1);
		return FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Backward.Left")));
	}

	else if(Forward_Backward_Movement_Value < 0.f && Right_Left_Movement_Value > 0.f)
	{
		Debug::Print("Parkour.Direction.Backward.Right", FColor::Purple, 1);
		return FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Backward.Right")));
	}

	else
	{
		Debug::Print("Parkour_Direction_None", FColor::Purple, 1);
		return FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.None")));
	}
}

void UCustom_Movement_Component::Parkour_Climb_Handle_Corner_Movement()
{

}

void UCustom_Movement_Component::Execute_Parkour_Action()
{
	if(Parkour_Action == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.No.Action"))))
	Parkour_Detect_Wall();
	
	if(Initial_Ground_Level_Front_Wall_Hit_Result.bBlockingHit)
	Grid_Scan_For_Hit_Results(Initial_Ground_Level_Front_Wall_Hit_Result.ImpactPoint, Reverse_Wall_Normal_Rotation_Z(Initial_Ground_Level_Front_Wall_Hit_Result.ImpactNormal), Grid_Scan_Width, Grid_Scan_Height);

	//"Grid_Scan_Hit_Traces" array is filled by the function call "Grid_Scan_For_Hit_Results()". 
	if(Grid_Scan_Hit_Traces.Num() != 0)
	{
		//This function analyzes the FHitResults stored in the array "Grid_Scan_Hit_Traces" for the line traces which are just under the top edge on the front side of the wall.
		//Said line traces are stored in the array "Front_Wall_Top_Edge_Traces".
		Analyze_Grid_Scan_For_Front_Wall_Top_Edge_Hits();
		
		//Empty the array because the information in it has been analyzed in "&UCustom_Movement_Component::Analyze_Grid_Scan_For_Front_Wall_Top_Edge_Hits()".
		//It needs to be empty so it can be filled with new information the next time it needs to be used.
		Grid_Scan_Hit_Traces.Empty();
	}
	
	//Front_Wall_Top_Edge_Traces are filled by the function call "Analyze_Grid_Scan_For_Front_Wall_Top_Edge_Hits()".
	if(Front_Wall_Top_Edge_Traces.Num() != 0)
	{
		//This function analyzes the line traces stored in the array "Front_Wall_Top_Edge_Traces" for the line trace which is closes to the character's current location.
		//Said line trace is stored in the global variable "Front_Wall_Top_Edge_Best_Hit{}".
		Realize_Front_Wall_Top_Edge_Best_Hit();

		//Empty the array because the information in it has been analyzed in "&UCustom_Movement_Component::Realize_Front_Wall_Top_Edge_Best_Hit()".
		//It needs to be empty so it can be filled with new information the next time it needs to be used.
		Front_Wall_Top_Edge_Traces.Empty();
	}

	//Global FhitResult variable "Front_Wall_Top_Edge_Best_Hit" is filled with the line trace that is just under the top edge on the front wall in the function call "Realize_Front_Wall_Top_Edge_Best_Hit()".
	//This check is to make sure said FHitResult does indeed have a blocking hit and no initial overlap is active. This FHitResult is used to analyze the top surface of the wall which in result will enable
	//the calculation of the location which the character will land on when vaulting.
	if(Front_Wall_Top_Edge_Best_Hit.bBlockingHit && !Front_Wall_Top_Edge_Best_Hit.bStartPenetrating)
	{
		Analyze_Wall_Top_Surface();
		Calculate_Wall_Vault_Location();
	}

	//Using the global FHitResults Wall_Top_Result (The first sphere trace (Index == 0) executed in the for loop found within function call "Analyze_Wall_Top_Surface()"), 
	//Wall_Depth_Result (The last sphere trace (Index != 0) executed in the for loop found within function call "Analyze_Wall_Top_Surface()" and Wall_Vault_Result (the sphere trace executed
	//in the function call "Calculate_Wall_Vault_Location()"), calculations are made to obtain the Wall_Height, Wall_Depth and Vault_Height.
	Measure_Wall();

	//Decide the Parkour Action to execute based on the current value of the global gameplaytag "Parkour_State" as well as the values stored in the global double variables
	//"Wall_Height", "Wall_Depth" and "Vault_Height".
	Decide_Parkour_Action();
}

#pragma endregion


#pragma endregion