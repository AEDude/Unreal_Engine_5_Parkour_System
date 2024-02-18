// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Custom_Movement_Component.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Debug/DebugHelper.h"
#include "Components/CapsuleComponent.h"
#include "Character/Technical_AnimatorCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionWarpingComponent.h"
#include "Engine/World.h"
#include "Gameplay_Tags/Gameplay_Tags.h"
#include "DrawDebugHelpers.h"
#include "Character_Direction/Character_Direction_Arrow.h"
#include "Data_Asset/Parkour_Action_Data.h"

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

	Owning_Player_Character = Cast<ATechnical_AnimatorCharacter>(CharacterOwner);
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

void UCustom_Movement_Component::Attach_Arrow_Actor_To_Character(ATechnical_AnimatorCharacter* Character)
{
	//Use the character pointer passed in at "&UCustom_Movement_Component::Initialize_Parkour_Pointers" by (ATechnical_AnimatorCharacter* Character) to 
	//"GetActorTransform()" and initialize input paramater 1. (for "GetWorld()->SpawnActor") "FTransform Location".
	FTransform Location{Character->GetActorTransform()};
	
	//Initialize Input parameter 2. (for "GetWorld()->SpawnActor") "FActorSpawnParameters Spawn_Info"
	FActorSpawnParameters Spawn_Info{};

	//Spawn the arrow component which is within "&ACharacter_Direction_Arrow" using "GetWorld()->SpawnActor". Use the two input parameters initialized above (Location, Spawn_Info).
	ACharacter_Direction_Arrow* Character_Direction_Arrow{GetWorld()->SpawnActor<ACharacter_Direction_Arrow>(ACharacter_Direction_Arrow::StaticClass(), Location, Spawn_Info)};
	
	//After spawning the arrow and attach it to the character using the character pointer passed in by (ATechnical_AnimatorCharacter* Character). Snap it to the target.
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

void UCustom_Movement_Component::Initialize_Parkour_Pointers(ATechnical_AnimatorCharacter* Character, UMotionWarpingComponent* Motion_Warping, UCameraComponent* Camera)
{
	//"GetCharacterMovement" using the character pointer passed in by (ATechnical_AnimatorCharacter* Character) and initialize "UCharacterMovementComponent* Character_Movement".
	Character_Movement = Character->GetCharacterMovement();
	//Get the mesh using the character pointer passed in by (ATechnical_AnimatorCharacter* Character) and initialize "USkeletalMeshComponent* Mesh".
	Mesh = Character->GetMesh(); 
	//Get the CapsuleComponent by using the character pointer passed in by (ATechnical_AnimatorCharacter* Character) and initialize "UCapsuleComponent* Capsule_Component".
	Capsule_Component = Character->GetCapsuleComponent();
	//Use the "USkeletalMeshComponent* Mesh" which is initialized by the character pointer passed in by (ATechnical_AnimatorCharacter* Character), to get the GetAnimInstance. Initialize "UAnimInstance* Anim_Instance".
	Anim_Instance = Mesh->GetAnimInstance();
	//Initialize "UMotionWarpingComponent* Motion_Warping_Component" with the "UMotionWarpingComponent* Motion_Warping" which is passed in by "&ATechnical_AnimatorCharacter". 
	Motion_Warping_Component = Motion_Warping;
	//Initialize "UCameraComponent* Camera_Component" with the "UCameraComponent* Camera" that is passed in by "&ATechnical_AnimatorCharacter".
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
	Draw_Debug_Sphere(Initial_Ground_Level_Front_Wall_Hit_Result.ImpactPoint, 5.f, FColor::Blue, 1.f, false, 7.f);
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
			Debug::Print(FString(TEXT("Differences in Front_Wall_Top_Edge_Traces: ") + FString::FromInt(Distance_Between_Current_And_Previous_Line_Trace)), FColor::Blue);
			Draw_Debug_Sphere(Previous_Iteration_Line_Trace_Reference.ImpactPoint, 5.f, FColor::Black, 10.f, false, 1);
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

	Draw_Debug_Sphere(Front_Wall_Top_Edge_Best_Hit.ImpactPoint, 10.f, FColor::Cyan, 5.f, false, 5.f);
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
			Draw_Debug_Sphere(Wall_Top_Result.ImpactPoint, 15.f, FColor::Emerald, 7.f, false, 7.f);
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
		Draw_Debug_Sphere(Wall_Depth_Result.ImpactPoint, 10.f, FColor::Purple, 10.f, false, 10.f);
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

	Draw_Debug_Sphere(Wall_Vault_Result.ImpactPoint, 10.f, FColor::Silver, 10.f, false, 10.f);
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
		Debug::Print(FString("Wall_Height: ") + FString::SanitizeFloat(Wall_Height));
	}

	else
	{
		Wall_Height = 0.f;
		Debug::Print(FString("Wall_Height: ") + FString::SanitizeFloat(Wall_Height));
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
		Debug::Print(FString("Wall_Depth: ") + FString::SanitizeFloat(Wall_Depth));
	}
		
	else
	{
		Wall_Depth = 0;
		Debug::Print(FString("Wall_Depth: ") + FString::SanitizeFloat(Wall_Depth));
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
		Debug::Print(FString("Vault_Height: ") + FString::SanitizeFloat(Vault_Height));
	}
		
	else
	{
		Vault_Height = 0;
		Debug::Print(FString("Vault_Height: ") + FString::SanitizeFloat(Vault_Height));
	} 
}

void UCustom_Movement_Component::Validate_Is_On_Ground()
{
	/*This function will be called every "Tick()" within the function "Parkour_Call_In_Tick()". The goal of this function is to determine whether the character is 
	grounded or airborne. By using a BoxTraceSingleForObjects that generates a box ray cast that is located at the same location as the root bone of the character 
	the result of whether there is a blocking hit or not stored in the local FHitResult variable "Out_Hit" can be used to set the value of the global bool variable 
	"Is_On_Ground".*/

	//If the current "Parkour_State" set on the character is "Parkour.State.Climb" then it is evident that the character is not grounded. In this case the global bool
	//variable "Is_On_Ground" will be set to false.
	if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))))
	{
		Is_On_Ground = false;
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
		Is_On_Ground = true;

		else
		Is_On_Ground = false;
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
		EDrawDebugTrace::ForDuration,
		Out_Hit,
		false
		);
	
	if(Out_Hit.bBlockingHit)
	Set_Parkour_Climb_Style(FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Climb.Style.Braced.Climb"))));

	else
	Set_Parkour_Climb_Style(FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Climb.Style.Free.Hang"))));
}

#pragma endregion

#pragma region Parkour_Core  

void UCustom_Movement_Component::Parkour_State_Settings(const ECollisionEnabled::Type& Collision_Type, const EMovementMode& New_Movement_Mode, const bool& bStop_Movement_Immediately)
{
	Capsule_Component->SetCollisionEnabled(Collision_Type);
	Character_Movement->SetMovementMode(New_Movement_Mode);
	
	if(bStop_Movement_Immediately) Character_Movement->StopMovementImmediately();
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
	if(Parkour_Climb_Direction != New_Climb_Direction)
	{
		Parkour_Climb_Direction = New_Climb_Direction;
		Parkour_Interface->Execute_Set_Climb_Direction(Anim_Instance, Parkour_Climb_Direction);
	}
	else return;
}

float UCustom_Movement_Component::Climb_Style_Values(const FGameplayTag& Climb_Style, const float& Braced_Value, const float& Free_Hang_Value) const
{
	const float& Parkour_Braced_Value{Braced_Value};
	const float& Parkour_Free_Hang_Value{Free_Hang_Value};

	if((Climb_Style == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Climb.Style.Braced.Climb")))))
	{
		return Parkour_Braced_Value;
	}

	else //if(Climb_Style == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Climb.Style.Free.Hang"))))
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
		Debug::Print(FString("Wall_Height: ") + FString::SanitizeFloat(Wall_Height));

		Wall_Depth = 0.f;
		Debug::Print(FString("Wall_Depth: ") + FString::SanitizeFloat(Wall_Depth));

		Vault_Height = 0.f;
		Debug::Print(FString("Vault_Height: ") + FString::SanitizeFloat(Vault_Height));
	}
 }

void UCustom_Movement_Component::Decide_Parkour_Action()
{
	if(Wall_Top_Result.bBlockingHit)
	{
		if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))))
		{
			Debug::Print("Climb_Or_Hop");	
		}

		else if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Free.Roam"))))
		{
			if(Wall_Height >= 90 && Wall_Height <= 170)
			{
				if(Wall_Depth >= 0 && Wall_Depth <= 120)
				{
					if(Vault_Height >= 70 && Vault_Height <= 120)
					{
						if(UpdatedComponent->GetComponentVelocity().Size() > 20)
						{
							Debug::Print("Parkour_Low_Vault");
						}

						else
						{
							Debug::Print("Parkour_Mantle");
						}

					}

					else if(Vault_Height >= 130 && Vault_Height <= 140)
					{
						if(UpdatedComponent->GetComponentVelocity().Size() > 20)
						{
							Debug::Print("Parkour_High_Vault");
						}

						else
						{
							Debug::Print("Parkour_No_Action");
							Set_Parkour_Action(FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.No.Action"))));
						}
						
					}

					else
					{
						Debug::Print("Parkour_No_Action");
						Set_Parkour_Action(FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.No.Action"))));
					}
				}

				else
				{
					Debug::Print("Parkour_Mantle");
				}
			}

			else
			{
				if(Wall_Height < 280)
				{
					
					Debug::Print("Parkour_Climb");
					Decide_Climb_Style(Wall_Top_Result.ImpactPoint, Reversed_Front_Wall_Normal_Z);
					
					/*The FHitResult stored in the global FHitResult variable "Wall_Top_Result" is copied to the the global 
					FHitResult variable New_Climb_Hit_Result so that when the function "Reset_Parkour_Variables()" is called
					after each Parkour Action is complete there will still be a location to begin the next sequence of ray casts.
					*/
					New_Climb_Hit_Result = Wall_Top_Result;
					
					if(Parkour_Climb_Style == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Climb.Style.Braced.Climb"))))
					{
						Set_Parkour_Action(FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.Braced.Climb"))));
					}
					
					else
					{
						Set_Parkour_Action(FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.Free.Hang"))));
					}
				}

				else
				{
					Debug::Print("No_Action");
					Set_Parkour_Action(FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.No.Action"))));
				}
			}
		}
	}
	
	else
	{
		Debug::Print("Parkour_No_Action");
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
	/*This function will be called every "Tick()". The goal of this function is to check whether the character is on the ground or not using the function call "Validate_Is_On_Ground()".
	Depending on the value set on the global bool variable "Is_On_Ground" within said function another check will be performed to check if the value set to the gameplay tag "Parkour_Action" 
	is equal to "Parkour.Action.No.Action". If this is the case, then the character is on the ground and is not performing any parkour. Therefore a call to reset the values stored in 
	the global FHitResults "Initial_Ground_Level_Front_Wall_Hit_Result", "Front_Wall_Top_Edge_Best_Hit", "Wall_Top_Result", "Wall_Depth_Result" and "Wall_Vault_Result" as well as the 
	double variables "Wall_Height", "Wall_Depth" and "Vault_Height". will be made. The resetting of said values will happen within the function call "Reset_Parkour_Variables()".*/

	Validate_Is_On_Ground();
	
	if(Is_On_Ground)
	{
		if(Parkour_Action == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.No.Action"))))
		Reset_Parkour_Variables();

		else
		return;
	}
	
	else
	{
		Debug::Print("In_Air");
		//if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Free.Roam"))))
		//Execute_Parkour_Action();
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
	Play_Parkour_Montage(Braced_Jump_To_Climb);

	else if(Parkour_Action == FGameplayTag::RequestGameplayTag("Parkour.Action.Free.Hang"))
	Play_Parkour_Montage(Free_Hang_Jump_To_Climb);

}

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
	return Debug::Print(TEXT("Parkour_State set from Parkour_Action_Data"));
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