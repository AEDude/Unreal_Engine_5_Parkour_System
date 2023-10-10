// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Custom_Movement_Component.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Technical_Animator/DebugHelper.h"
#include "Components/CapsuleComponent.h"
#include "Technical_Animator/Technical_AnimatorCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Technical_Animator/Technical_AnimatorCharacter.h"
#include "MotionWarpingComponent.h"


void UCustom_Movement_Component::BeginPlay()
{
	Super::BeginPlay();

	Owning_Player_Animation_Instance = CharacterOwner->GetMesh()->GetAnimInstance();

	if(Owning_Player_Animation_Instance)
	{
		Owning_Player_Animation_Instance->OnMontageEnded.AddDynamic(this, &UCustom_Movement_Component::On_Climbing_Montage_Ended);
		Owning_Player_Animation_Instance->OnMontageBlendingOut.AddDynamic(this, &UCustom_Movement_Component::On_Climbing_Montage_Ended);
	}

	Owning_Player_Character = Cast<ATechnical_AnimatorCharacter>(CharacterOwner);
}

void UCustom_Movement_Component::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const FVector Unrotated_Last_Input_Vector = 
	UKismetMathLibrary::Quat_UnrotateVector(UpdatedComponent->GetComponentQuat(), GetLastInputVector());

	Debug::Print(Unrotated_Last_Input_Vector.GetSafeNormal().ToCompactString(), FColor:: Cyan, 9);


	/*Trace_Climbable_Surfaces();

	Trace_From_Eye_Height(100.f);*/
}

void UCustom_Movement_Component::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	if(Is_Climbing())
	{
		bOrientRotationToMovement = false;
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(48.f);
		On_Enter_Climb_State_Delegate.ExecuteIfBound();
	}

	if(PreviousMovementMode == MOVE_Custom && PreviousCustomMode == E_Custom_Movement_Mode::MOVE_Climb)
	{
		bOrientRotationToMovement = true;
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(96.f);

		const FRotator Dirty_Rotation = UpdatedComponent->GetComponentRotation();
		const FRotator Clean_Stand_Rotation = FRotator(0.f, Dirty_Rotation.Yaw, 0.f);
		UpdatedComponent->SetRelativeRotation(Clean_Stand_Rotation);

		StopMovementImmediately();

		On_Exit_Climb_State_Delegate.ExecuteIfBound();
	}

	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

void UCustom_Movement_Component::PhysCustom(float deltaTime, int32 Iterations)
{
	if(Is_Climbing())
	{
		Physics_Climb(deltaTime, Iterations);
	}

	Super::PhysCustom(deltaTime, Iterations);
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
}

float UCustom_Movement_Component::GetMaxAcceleration() const
{
    if(Is_Climbing())
	{
		return Max_Climb_Acceleration;
	}
	
	else
	
	{
		return Super:: GetMaxAcceleration();
	}
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
		CalcVelocity(deltaTime, 0.f, true, Max_Break_Climb_Deceleration);
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

	const FVector SnapVector = -Current_Climbable_Surface_Normal * Projected_Character_To_Surface.Length();

	UpdatedComponent->MoveComponent(
	SnapVector * DeltaTime * Max_Climb_Speed,
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

	Owning_Player_Character->GetMotion_Warping_Component()->AddOrUpdateWarpTargetFromLocation(
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