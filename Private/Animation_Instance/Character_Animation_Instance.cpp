// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation_Instance/Character_Animation_Instance.h"
#include "Character/Technical_Animator_Character.h"
#include "Components/Custom_Movement_Component.h"
#include "Kismet/KismetMathLibrary.h"
#include "Debug/DebugHelper.h"
#include "Kismet/GameplayStatics.h"
#include "KismetAnimationLibrary.h"
#include "AnimCharacterMovementLibrary.h"


void UCharacter_Animation_Instance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    Technical_Animator_Character = Cast<ATechnical_Animator_Character>(TryGetPawnOwner());

    if(Technical_Animator_Character)
    {
        Custom_Movement_Component = Technical_Animator_Character->Get_Custom_Movement_Component();
        Primary_Rotation = Technical_Animator_Character->GetActorRotation();
        Mesh = Technical_Animator_Character->GetMesh();
    }  
}

void UCharacter_Animation_Instance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

    Update_Variables_On_Secondary_Thread(DeltaSeconds);
}

void UCharacter_Animation_Instance::NativeUpdateAnimation(float DeltaSeconds)
{
    if((!Technical_Animator_Character || Parkour_State != FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Free.Roam"))) || 
	Parkour_State != FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Stairs")))) &&
    Parkour_Action != FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.No.Action"))))
    {
        return;
    } 
   
    Super::NativeUpdateAnimation(DeltaSeconds);


    Get_Dynamic_Look_Offset_Values(DeltaSeconds);

    Dynamic_Look_Offset_Weight(DeltaSeconds);
    
    Find_Ground_Locomotion_State();
   
    FString String{*UEnum::GetValueAsString(Ground_Locomotion_State)};
    Debug::Print("Current_Ground_Locomotion_State_Is: " + String, FColor::MakeRandomColor(), 3);
    Debug::Print("Current_Acceleration: " + FString::SanitizeFloat(Acceleration.Length()), FColor::MakeRandomColor(), 4);

    Track_Ground_Locomotion_State_Idle(EGround_Locomotion_State::EGLS_Idle);
    Debug::Print("Idle_Locomotion_Start_Angle: " + FString::FromInt(Locomotion_Start_Angle), FColor::Yellow, 7);
    
    Track_Ground_Locomotion_State_Walking(EGround_Locomotion_State::EGLS_Walking);
    Debug::Print("Walking_Locomotion_Start_Angle: " + FString::FromInt(Locomotion_Start_Angle), FColor::Yellow, 7);

    Track_Ground_Locomotion_State_Jogging(EGround_Locomotion_State::EGLS_Jogging);
    Debug::Print("Jogging_Locomotion_Start_Angle: " + FString::FromInt(Locomotion_Start_Angle), FColor::Yellow, 7);

    Calculate_Dynamic_Lean_Angle();
    

}

void UCharacter_Animation_Instance::NativePostEvaluateAnimation()
{
    Super::NativePostEvaluateAnimation();

    if((Parkour_State != FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Free.Roam"))) || 
	Parkour_State != FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Stairs")))) &&
    Parkour_Action != FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.No.Action"))))
    {
       return; 
    } 
    
    Update_Character_Rotation();
}

void UCharacter_Animation_Instance::Update_Variables_On_Secondary_Thread(const float& DeltaSeconds)
{
    if(!Technical_Animator_Character || !Custom_Movement_Component)
    {
        return;
    }
    
    Get_Is_Crouching();
    
    Get_Input_Vector();

    Get_Acceleration();

    Get_Ground_Speed();

    Get_Air_Speed();

    Get_Velocity();

    Get_Is_Jogging();

    Calculate_Direction();

    Get_Predicted_Stop_Distance_Variables();

    Get_Should_Move();

    Get_Is_Falling();

    if(Custom_Movement_Component)
    {
        Forward_Backward_Movement_Value = Custom_Movement_Component->Forward_Backward_Movement_Value;

        Right_Left_Movement_Value = Custom_Movement_Component->Right_Left_Movement_Value;

        bCan_Initialize_Running_Start = Custom_Movement_Component->Get_bCan_Initialize_Running_Start();
    }

    // Get_Is_Climbing();

    // Get_Climb_Velocity();

    // Get_Is_Taking_Cover();

    // Get_Take_Cover_Velocity();
}


#pragma region Custom_Locomotion_Helper

void UCharacter_Animation_Instance::Get_Is_Crouching()
{
    if(Technical_Animator_Character)
    {
        bIs_Crouching = Technical_Animator_Character->bIsCrouched;
    }
}

void UCharacter_Animation_Instance::Get_Input_Vector()
{
   if(Custom_Movement_Component)
   {
        Input_Vector = Custom_Movement_Component->GetLastInputVector();
   }
}

void UCharacter_Animation_Instance::Get_Acceleration()
{
   if(Custom_Movement_Component)
   {
    //This is the value you see when you look at the animation state machine. The value set to the acceleration determines whether your character is idle, walking, jogging or sprinting.
    Acceleration = Custom_Movement_Component->GetCurrentAcceleration();

    Acceleration = Acceleration.GetSafeNormal();
   }
}

void UCharacter_Animation_Instance::Get_Ground_Speed()
{
   if(Technical_Animator_Character)
   {
    //This is the velocity of the characters movement on the global X and Y axis. The Air velocity (Z axis) has been omitted.
    Ground_Speed = UKismetMathLibrary::VSizeXY(Technical_Animator_Character->GetVelocity());
   }
}

void UCharacter_Animation_Instance::Get_Air_Speed()
{
   if(Technical_Animator_Character)
   {
        Air_Speed = Technical_Animator_Character->GetVelocity().Z;
   }
   
}

void UCharacter_Animation_Instance::Get_Velocity()
{
    if(Technical_Animator_Character)
    {
       //The velocity is a vector that points in the direction in which the character is moving
        Velocity = Technical_Animator_Character->GetVelocity();

        Velocity = Velocity.GetSafeNormal(); 
    } 
}

void UCharacter_Animation_Instance::Calculate_Direction()
{
    if(Technical_Animator_Character)
    {
       Direction_For_Orientation_Warping = UKismetAnimationLibrary::CalculateDirection(Velocity, Technical_Animator_Character->GetActorRotation()); 
    }
}

void UCharacter_Animation_Instance::Get_Is_Jogging()
{
    if(Technical_Animator_Character)
    {
        bIs_Jogging = Technical_Animator_Character->Get_Is_Jogging();
    }

    else
    {
        bIs_Jogging = false;
    }
}

void UCharacter_Animation_Instance::Get_Should_Move()
{
    if(Custom_Movement_Component)
    {
       bShould_Move =
        Custom_Movement_Component->GetCurrentAcceleration().Size()>0 && 
        Ground_Speed>5.f &&
        !bIs_Falling; 
    }
}

void UCharacter_Animation_Instance::Get_Is_Falling()
{
    bIs_Falling = Custom_Movement_Component->IsFalling();
}

void UCharacter_Animation_Instance::Get_Is_Climbing()
{
    bIs_Climbing = Custom_Movement_Component->Is_Climbing();
}

void UCharacter_Animation_Instance::Get_Climb_Velocity()
{
  Climb_Velocity = Custom_Movement_Component->Get_Unrotated_Climb_Velocity();
}

void UCharacter_Animation_Instance::Get_Is_Taking_Cover()
{
    bIs_Taking_Cover = Custom_Movement_Component->Is_Taking_Cover();
}

void UCharacter_Animation_Instance::Get_Take_Cover_Velocity()
{
    Take_Cover_Velocity = Custom_Movement_Component->Get_Unrotated_Take_Cover_Velocity();
}

#pragma endregion

#pragma region Custom_Ground_Locomotion_Core

void UCharacter_Animation_Instance::Find_Ground_Locomotion_State()
{
    if(!Technical_Animator_Character)
    {
       return; 
    }
    
    //Get the dot product between the acceleration and the velocity. This is used to determine whether the character should pivot or not (change direction abruptly).
    //When the acceleration value is equal to 0 (the input from the controller is commanding the character to go in a different direction than the direction of the velocity).
    //Therefore the character will begin braking and in result the dot product will be -1.
    const double Should_Character_Pivot_Dot_Product{FVector::DotProduct(Velocity, Acceleration)};
    Debug::Print("Should_Character_Pivot_Dot_Product: " + FString::SanitizeFloat(Should_Character_Pivot_Dot_Product), FColor::Yellow, 7);

    if(Should_Character_Pivot_Dot_Product < 0.f)
    {
        Ground_Locomotion_State = EGround_Locomotion_State::EGLS_Idle; 
        Debug::Print("Change_Direction_Abruptly_By_Pivoting", FColor::MakeRandomColor(), 1);
    }

    //Determine if character should be idle
    if(Ground_Speed == 0.f || Acceleration.Length() == 0.f)
    {
        Ground_Locomotion_State = EGround_Locomotion_State::EGLS_Idle; 
        Debug::Print("Ground_Locomotion_State_Is_Idle", FColor::MakeRandomColor(), 1);
    }

    //Determine if character should walk
    else if(Ground_Speed >= .01f && !Technical_Animator_Character->Get_Is_Jogging())
    {
        Ground_Locomotion_State = EGround_Locomotion_State::EGLS_Walking;
        Debug::Print("Ground_Locomotion_State_Is_Walking", FColor::MakeRandomColor(), 1);
    }

    //Determine if character should jog
    else if(Ground_Speed > 241.f || Technical_Animator_Character->Get_Is_Jogging())
    {
        Ground_Locomotion_State = EGround_Locomotion_State::EGLS_Jogging;
        Debug::Print("Ground_Locomotion_State_Is_Jogging", FColor::MakeRandomColor(), 1);
    }
}

void UCharacter_Animation_Instance::Idle_Turn_In_Place()
{
    if(!Technical_Animator_Character)
    {
        return;
    }
    
    Turn_In_Place_Starting_Rotation = Technical_Animator_Character->GetActorRotation();
    Turn_In_Place_Delta = UKismetMathLibrary::NormalizedDeltaRotator(Primary_Rotation, Turn_In_Place_Starting_Rotation).Yaw;
    
    if(UKismetMathLibrary::Abs(Turn_In_Place_Delta) > Turn_In_Place_Minimum_Threshold)
    {
        bCan_Turn_In_Place = true;
        bDisable_Turn_In_Place = false;

        Turn_In_Place_Target_Angle = Turn_In_Place_Delta;

        if(bTurn_In_Place_Flip_Flop)
        {
            bTurn_In_Place_Flip_Flop = false;
            bDisable_Turn_In_Place = true;
        }

        else if(!bTurn_In_Place_Flip_Flop)
        {
            bTurn_In_Place_Flip_Flop = true;
            bDisable_Turn_In_Place = true;
        }
    }

    else
    {
        bCan_Turn_In_Place = false;
        bDisable_Turn_In_Place = true;
    }
}

void UCharacter_Animation_Instance::Update_Rotation_Turn_In_Place()
{
    if(!Technical_Animator_Character)
    {
        return;
    }

    else if(bCan_Turn_In_Place && GetCurveValue(FName(TEXT("Turn_In_Place"))) > 0.f)
    {
        Turn_In_Place_Starting_Rotation = Technical_Animator_Character->GetActorRotation();

        const double Characters_Rotation_After_Turning_In_Place{
            ((GetCurveValue(FName(TEXT("Rotation_Turn_In_Place"))) * Turn_In_Place_Target_Angle / 17.f) + 
            Turn_In_Place_Starting_Rotation.Yaw)};

        Technical_Animator_Character->SetActorRotation(FRotator(0.f, Characters_Rotation_After_Turning_In_Place, 0.f));

        Turn_In_Place_Delta = UKismetMathLibrary::NormalizedDeltaRotator(Primary_Rotation, Turn_In_Place_Starting_Rotation).Yaw; 
    }

    if(GetCurveValue(FName(TEXT("Rotation_Turn_In_Place"))) >= 1.f)
    {
        bCan_Turn_In_Place = false;
    }

}

void UCharacter_Animation_Instance::Track_Ground_Locomotion_State_Idle(const EGround_Locomotion_State& Ground_Locomotion_State_Reference)
{
    if(Ground_Locomotion_State_Reference == Ground_Locomotion_State)
    {
        if(Do_Once_1)
        {
            //Reset
            Do_Once_2 = true;

            //On_Enter
            Debug::Print("Update_On_Idle_Enter", FColor::Yellow, 5);
            Idle_Turn_In_Place();

            Do_Once_1 = false;
        }

        else
        {
            //While_True
            Debug::Print("Idle_Active", FColor::Green, 5);
        }
    }

    else
    {
        if(Do_Once_2)
        {
            //Reset
            Do_Once_1 = true;
            
            //On_Exit
            Debug::Print("Exiting_Idle", FColor::Yellow, 5);

            Do_Once_2 = false;
        }

        else
        {
            //While_False
            Debug::Print("Idle_Inactive", FColor::Red, 5);
        }
    }
}

void UCharacter_Animation_Instance::Track_Ground_Locomotion_State_Walking(const EGround_Locomotion_State& Ground_Locomotion_State_Reference)
{
    if(Ground_Locomotion_State_Reference == Ground_Locomotion_State)
    {
        if(Do_Once_1)
        {
            //Reset
            Do_Once_2 = true;

            //On_Enter
            Debug::Print("Update_On_Walking_Enter", FColor::Yellow, 5);
            Update_On_Movement_Enter();
            Update_Locomotion_Play_Rate();


            Do_Once_1 = false;
        }

        else
        {
            //While_True
            Debug::Print("Wasling_Active", FColor::Green, 5);
            Update_Locomotion_Play_Rate();
        }
    }

    else
    {
        if(Do_Once_2)
        {
            //Reset
            Do_Once_1 = true;
            
            //On_Exit
            Debug::Print("Exiting_Walking", FColor::Yellow, 5);

            Do_Once_2 = false;
        }

        else
        {
            //While_False
            Debug::Print("Walking_Inactive", FColor::Red, 5);
        }
    }
}

void UCharacter_Animation_Instance::Track_Ground_Locomotion_State_Jogging(const EGround_Locomotion_State& Ground_Locomotion_State_Reference)
{
    if(Ground_Locomotion_State_Reference == Ground_Locomotion_State)
    {
        if(Do_Once_1)
        {
            //Reset
            Do_Once_2 = true;

            //On_Enter
            Debug::Print("Update_On_Jogging_Enter", FColor::Yellow, 5);
            Update_On_Movement_Enter();
            Update_Locomotion_Play_Rate();
            
            Do_Once_1 = false;
        }

        else
        {
            //While_True
            Debug::Print("Jogging_Active", FColor::Green, 5);
            Update_Locomotion_Play_Rate();
        }
    }

    else
    {
        if(Do_Once_2)
        {
            //Reset
            Do_Once_1 = true;
            
            //On_Exit
            Debug::Print("Exiting_Jogging", FColor::Yellow, 5);

            Do_Once_2 = false;
        }

        else
        {
            //While_False
            Debug::Print("Jogging_Inactive", FColor::Red, 5);
        }   
    }
}

void UCharacter_Animation_Instance::Find_Locomotion_Start_Direction(const float& Starting_Angle)
{
    if(UKismetMathLibrary::InRange_FloatFloat(Starting_Angle, -70.f, 70.f, true, true))
    {
        Ground_Locomotion_Starting_Direction = EGround_Locomotion_Starting_Direction::EGLSD_Forward;
    }

    else if(UKismetMathLibrary::InRange_FloatFloat(Starting_Angle, 70, 130.f, true, true))
    {
       Ground_Locomotion_Starting_Direction = EGround_Locomotion_Starting_Direction::EGLSD_Right;
    }
    
    else if(UKismetMathLibrary::InRange_FloatFloat(Starting_Angle, -130.f, -70.f, true, true))
    {
        Ground_Locomotion_Starting_Direction = EGround_Locomotion_Starting_Direction::EGLSD_Left;
    }

    else
    {
        if(UKismetMathLibrary::InRange_FloatFloat(Starting_Angle, 130.f, 180.f, true, true))
        {
            Ground_Locomotion_Starting_Direction = EGround_Locomotion_Starting_Direction::EGLSD_Backward_Right;
        }

        else
        {
            Ground_Locomotion_Starting_Direction = EGround_Locomotion_Starting_Direction::EGLSD_Backward_Left;
        }
    }
}

void UCharacter_Animation_Instance::Update_On_Movement_Enter()
{
    if(!Technical_Animator_Character)
    {
        return;
    }
    
    Starting_Rotation = Technical_Animator_Character->GetActorRotation();

    Primary_Rotation = UKismetMathLibrary::MakeRotFromX(Input_Vector); 

    Secondary_Rotation = Primary_Rotation;

    const FRotator Delta_Rotation_Between_Starting_Rotation_And_Primary_Rotation{UKismetMathLibrary::NormalizedDeltaRotator(Primary_Rotation, Starting_Rotation)};

    Locomotion_Start_Angle = Delta_Rotation_Between_Starting_Rotation_And_Primary_Rotation.Yaw;

    Find_Locomotion_Start_Direction(Locomotion_Start_Angle);

    FString String{*UEnum::GetValueAsString(Ground_Locomotion_Starting_Direction)};
    Debug::Print("Ground_Locomotion_Starting_Direction_Is: " + String, FColor::MakeRandomColor(), 8);

    return;
}

void UCharacter_Animation_Instance::Update_Character_Rotation()
{
    if(Ground_Locomotion_State == EGround_Locomotion_State::EGLS_Idle)
    {
        Update_Rotation_Turn_In_Place();
    }

    else
    {
        Update_Character_Rotation_While_Moving();
    }
}

void UCharacter_Animation_Instance::Update_Character_Rotation_While_Moving()
{
    if(Technical_Animator_Character && 
    Parkour_State != FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Free.Roam"))) && 
    Parkour_Action != FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.No.Action"))))
    {
        const double Delta_Time{UGameplayStatics::GetWorldDeltaSeconds(this)};
    
        Primary_Rotation = UKismetMathLibrary::RInterpTo_Constant(Primary_Rotation, UKismetMathLibrary::MakeRotFromX(Input_Vector), Delta_Time, 1000.f);

        Secondary_Rotation = UKismetMathLibrary::RInterpTo_Constant(Secondary_Rotation, Primary_Rotation, Delta_Time, 7.f);

        const double Rotation_Remapped{UKismetMathLibrary::MapRangeClamped(GetCurveValue(FName(TEXT("Rotation"))), 0.f, 1.f, 1.f, 0.f) * -1 * Locomotion_Start_Angle + Secondary_Rotation.Yaw};
    
        Technical_Animator_Character->SetActorRotation(FRotator(0.f, Rotation_Remapped, 0.f));
    }
}

void UCharacter_Animation_Instance::Update_Locomotion_Play_Rate()
{
    //UKismetMathLibrary::Clamp(UKismetMathLibrary::SafeDivide(Ground_Speed, GetCurveValue(FName(TEXT("Moving_Speed")))), .5f, 1.7f);
    
    if(Ground_Speed >= 170.f)
    {
      Animation_Play_Rate = 1.f;
    }

    else if(Ground_Speed >= 100.f && Ground_Speed <= 170.f)
    {
        Animation_Play_Rate = 1;
    }
    
    else if(Ground_Speed >= 0.f && Ground_Speed <= 100.f)
    {
        Animation_Play_Rate = .5;
    }

}

void UCharacter_Animation_Instance::Get_Predicted_Stop_Distance_Variables()
{
    if(!Custom_Movement_Component)
    {
        return;
    }

    else
    {
        bUse_Seperate_Braking_Friction = Custom_Movement_Component->bUseSeparateBrakingFriction;

        Braking_Friction = Custom_Movement_Component->BrakingFriction;

        Ground_Friction = Custom_Movement_Component->GroundFriction;

        Braking_Friction_Factor = Custom_Movement_Component->BrakingFrictionFactor;

        Braking_Deceleration_Walking = Custom_Movement_Component->BrakingDecelerationWalking;
    
        if(Ground_Locomotion_State == EGround_Locomotion_State::EGLS_Idle)
        {
            Distance_To_Match = UAnimCharacterMovementLibrary::PredictGroundMovementStopLocation(Velocity, bUse_Seperate_Braking_Friction, Braking_Friction, 
                                                                                                 Ground_Friction, Braking_Friction_Factor, Braking_Deceleration_Walking).Length(); 
        }

        else
        {
            Distance_To_Match = 0.f;
        }
    }

}

void UCharacter_Animation_Instance::Get_Dynamic_Look_Offset_Values(const float& DeltaSeconds)
{
    if(!Custom_Movement_Component || !Technical_Animator_Character)
    {
        return;
    }
    
    else
    {
        if((Parkour_State != FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Free.Roam"))) || 
	    Parkour_State != FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Stairs")))) ||
        (Parkour_Action != FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.No.Action"))) ||
        (UKismetMathLibrary::Abs(Forward_Backward_Movement_Value) == 0 && UKismetMathLibrary::Abs(Right_Left_Movement_Value) == 0)))
        {
            Left_Right_Look_Value = UKismetMathLibrary::FInterpTo(Look_At_Value_Final_Interpolation, 0, DeltaSeconds, 3);
        }

        if(Technical_Animator_Character->Get_Is_Jogging())
        {
            Left_Right_Look_Value = UKismetMathLibrary::FInterpTo(Look_At_Value_Final_Interpolation, 0, DeltaSeconds, 3);
        }
        
        Current_Input_Vector = FVector(Right_Left_Movement_Value, Forward_Backward_Movement_Value, 0);

	    Current_Input_Rotation = UKismetMathLibrary::MakeRotFromX(Current_Input_Vector);

	    Target_Input_Rotation = UKismetMathLibrary::RInterpTo_Constant(Target_Input_Rotation, Current_Input_Rotation, DeltaSeconds, 200.f);
	    Interpolated_Direction = UKismetMathLibrary::GetForwardVector(Target_Input_Rotation);
        if(bLook_Left_Right_Debug_Visibility)
	    UKismetSystemLibrary::DrawDebugCoordinateSystem(this, Mesh->GetSocketLocation(FName(TEXT("pelvis"))), Target_Input_Rotation, 150.f, 0.f, 1.f);

        Target_Input_Rotation_1 = UKismetMathLibrary::RInterpTo_Constant(Target_Input_Rotation_1, Current_Input_Rotation, DeltaSeconds, 190.f);
	    Interpolated_Direction = UKismetMathLibrary::GetForwardVector(Target_Input_Rotation_1);
        if(bLook_Left_Right_Debug_Visibility)
	    UKismetSystemLibrary::DrawDebugCoordinateSystem(this, Mesh->GetSocketLocation(FName(TEXT("spine_01"))), Target_Input_Rotation_1, 150.f, 0.f, 1.f);

        Target_Input_Rotation_2 = UKismetMathLibrary::RInterpTo_Constant(Target_Input_Rotation_2, Current_Input_Rotation, DeltaSeconds, 170.f);
	    Interpolated_Direction = UKismetMathLibrary::GetForwardVector(Target_Input_Rotation_2);
        if(bLook_Left_Right_Debug_Visibility)
	    UKismetSystemLibrary::DrawDebugCoordinateSystem(this, Mesh->GetSocketLocation(FName(TEXT("spine_02"))), Target_Input_Rotation_2, 150.f, 0.f, 1.f);

        Target_Input_Rotation_3 = UKismetMathLibrary::RInterpTo_Constant(Target_Input_Rotation_3, Current_Input_Rotation, DeltaSeconds, 120.f);
	    Interpolated_Direction = UKismetMathLibrary::GetForwardVector(Target_Input_Rotation_3);
        if(bLook_Left_Right_Debug_Visibility)
	    UKismetSystemLibrary::DrawDebugCoordinateSystem(this, Mesh->GetSocketLocation(FName(TEXT("spine_03"))), Target_Input_Rotation_3, 150.f, 0.f, 1.f);

        Target_Input_Rotation_4 = UKismetMathLibrary::RInterpTo_Constant(Target_Input_Rotation_4, Current_Input_Rotation, DeltaSeconds, 70.f);
	    Interpolated_Direction = UKismetMathLibrary::GetForwardVector(Target_Input_Rotation_4);
        if(bLook_Left_Right_Debug_Visibility)
	    UKismetSystemLibrary::DrawDebugCoordinateSystem(this, Mesh->GetSocketLocation(FName(TEXT("spine_04"))), Target_Input_Rotation_4, 150.f, 0.f, 1.f);

        // Target_Input_Rotation_5 = UKismetMathLibrary::RInterpTo_Constant(Target_Input_Rotation_5, Current_Input_Rotation, DeltaSeconds, 55.f);
	    // Interpolated_Direction = UKismetMathLibrary::GetForwardVector(Target_Input_Rotation_5);
        //if(bLook_Left_Right_Debug_Visibility)
	    // UKismetSystemLibrary::DrawDebugCoordinateSystem(this, Mesh->GetSocketLocation(FName(TEXT("spine_05"))), Target_Input_Rotation_5, 150.f, 0.f, 1.f);

        // Target_Input_Rotation_5_5 = UKismetMathLibrary::RInterpTo_Constant(Target_Input_Rotation_5_5, Current_Input_Rotation, DeltaSeconds, 35.f);
	    // Interpolated_Direction = UKismetMathLibrary::GetForwardVector(Target_Input_Rotation_5_5);
        //if(bLook_Left_Right_Debug_Visibility)
	    // UKismetSystemLibrary::DrawDebugCoordinateSystem(this, Mesh->GetSocketLocation(FName(TEXT("neck_01"))), Target_Input_Rotation_5_5, 150.f, 0.f, 1.f);

        // Target_Input_Rotation_7 = UKismetMathLibrary::RInterpTo_Constant(Target_Input_Rotation_7, Current_Input_Rotation, DeltaSeconds, 15.f);
	    // Interpolated_Direction = UKismetMathLibrary::GetForwardVector(Target_Input_Rotation_7);
        //if(bLook_Left_Right_Debug_Visibility)
	    // UKismetSystemLibrary::DrawDebugCoordinateSystem(this, Mesh->GetSocketLocation(FName(TEXT("head"))), Target_Input_Rotation_7, 150.f, 0.f, 1.f);
    
        const double Initial_Left_Right_Look_Value_Raw{Interpolated_Direction.X * 
                                                   Interpolated_Direction.Y * 
                                                   Technical_Animator_Character->GetBaseAimRotation().Yaw / 77};
    
        const double Look_At_Value_Clamped{UKismetMathLibrary::ClampAngle(Left_Right_Look_Value, -1, 1)};

        Look_At_Value_Final_Interpolation = UKismetMathLibrary::FInterpTo(Initial_Left_Right_Look_Value_Raw, Look_At_Value_Clamped, DeltaSeconds, 10);

        Left_Right_Look_Value = Look_At_Value_Final_Interpolation;
	

    
        // const FRotator Look_Rotation{Technical_Animator_Character->Get_Left_Right_Look_Value()};

        // const FRotator Movement_Rotation{UKismetMathLibrary::MakeRotFromX(Technical_Animator_Character->GetActorForwardVector())};

        // const FRotator Normalized_Delta_Rotation{UKismetMathLibrary::NormalizedDeltaRotator(Movement_Rotation, Look_Rotation)};

        // const FRotator Delta_Rotation_Shorest_Path{FMath::RInterpTo(Delta_Rotation_Shorest_Path, Normalized_Delta_Rotation, DeltaSeconds, 1.f)};

        // Left_Right_Look_Value = Delta_Rotation_Shorest_Path.Yaw;

        Debug::Print("Left_Right_Look_Value: " + FString::SanitizeFloat(Left_Right_Look_Value), FColor::Green, 10);
        // Debug::Print("Up_Down_Look_Value: " + FString::SanitizeFloat(Up_Down_Look_Value), FColor::Green, 11);
    }
}

void UCharacter_Animation_Instance::Dynamic_Look_Offset_Weight(const float& DeltaSeconds)
{
    if(!Technical_Animator_Character)
    {
        return;
    }
   
    else if((Parkour_State != FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Free.Roam"))) || 
	Parkour_State != FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Stairs")))) &&
    (Parkour_Action != FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Action.No.Action"))) || 
    !Technical_Animator_Character->Get_Is_Jogging()))
    {
       Dynamic_Look_Weight = 25.f; 
    } 
    
    else
    {
        Dynamic_Look_Weight = UKismetMathLibrary::FInterpTo(Look_At_Value_Final_Interpolation, 0, DeltaSeconds, 3.f);
    }

}

void UCharacter_Animation_Instance::Calculate_Dynamic_Lean_Angle()
{
    if(!Technical_Animator_Character)
    {
       return; 
    }
    
    //Set the value for the character yaw during the previous frame with the value that is set within the global float variable "Character_Yaw". 
    //This will be subtracted from the yaw of the current frame to get the delta between the two variables which will then be used to get the rate of change.. 
    const float Character_Yaw_During_Last_Tick{Character_Yaw};

    //Set the value for the character yaw during the current frame into the global variable Character_Yaw.
    Character_Yaw = Technical_Animator_Character->GetActorRotation().Yaw;

    //Get the delta between the value set within the local float variable "Character_Yaw_During_Last_Tick" and the value set within the global float variable 
    //"Character_Yaw".
    const float Yaw_Delta_Between_Current_Tick_And_Previous_Tick{Character_Yaw - Character_Yaw_During_Last_Tick};

    /*Calculate the "Dynamic_Lean_Angle" (rate of change in the yaw of the character) by safe dividing the value within the local float variable 
    "Yaw_Delta_Between_Current_Tick_And_Previous_Tick" by "DeltaTime". SafeDivide will make sure that if the value is set to "0" no exceptions will be launched.
    Once the value is calculated clamp said value between -1 and 1 w/ -1 reprenting the character rotating/turning (yaw) to the left while running or walking
    and 1 representing the character rotating/turning (yaw) to the right while running or walking. */

    const double DeltaTime{UGameplayStatics::GetWorldDeltaSeconds(this)};

    Dynamic_Lean_Angle = UKismetMathLibrary::Clamp(UKismetMathLibrary::SafeDivide(Yaw_Delta_Between_Current_Tick_And_Previous_Tick, DeltaTime), -1, 1);

    Debug::Print("Dynamic_Lean_Angle: " + FString::SanitizeFloat(Dynamic_Lean_Angle), FColor::Green, 70);

}

#pragma endregion

#pragma region Parkour_Interface

#pragma region Parkour_Locomotion

void UCharacter_Animation_Instance::Set_Parkour_State_Implementation(const FGameplayTag &New_Parkour_State)
{
   Parkour_State = New_Parkour_State;
}

void UCharacter_Animation_Instance::Set_Parkour_Action_Implementation(const FGameplayTag &New_Parkour_Action)
{
    Parkour_Action = New_Parkour_Action;
}

void UCharacter_Animation_Instance::Set_Parkour_Climb_Style_Implementation(const FGameplayTag &New_Climb_Style)
{
   Parkour_Climb_Style = New_Climb_Style;
}

void UCharacter_Animation_Instance::Set_Parkour_Wall_Run_Side_Implementation(const FGameplayTag& New_Wall_Run_Side)
{
    Parkour_Wall_Run_Side = New_Wall_Run_Side;
}

void UCharacter_Animation_Instance::Set_Parkour_Direction_Implementation(const FGameplayTag& New_Parkour_Direction)
{
    Parkour_Direction = New_Parkour_Direction;

    if(Parkour_Direction == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Forward"))) || 
       Parkour_Direction == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Forward.Right"))) ||
       Parkour_Direction == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Right"))))
    {
        Left_Hand_Curve_Alpha = 1.f;
        Right_Hand_Curve_Alpha = 0.f;
    }
    
    else if((Parkour_Direction == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Left")))) ||
            (Parkour_Direction == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Forward.Left")))))
    {
        Left_Hand_Curve_Alpha = 0.f;
        Right_Hand_Curve_Alpha = 1.f;
    }

    else if(Parkour_Direction == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Backward"))) ||
            Parkour_Direction == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Backward.Left"))) ||
            Parkour_Direction == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Backward.Right"))))   
    {
        Left_Hand_Curve_Alpha = 1.f;
        Right_Hand_Curve_Alpha = 1.f;
    }
            

    Debug::Print("Parkour_Direction: " + Parkour_Direction.ToString(), FColor::MakeRandomColor(), 2000);
}

void UCharacter_Animation_Instance::Set_Parkour_Stairs_Direction_Implementation(const FGameplayTag& New_Parkour_Stairs_Direction)
{
    Parkour_Stairs_Direction = New_Parkour_Stairs_Direction;
    
}

void UCharacter_Animation_Instance::Set_Parkour_Slide_Side_Implementation(const FGameplayTag& New_Parkour_Slide_Side)
{
    Parkour_Slide_Side = New_Parkour_Slide_Side;
}

#pragma endregion

#pragma region Limbs_Location_And_Rotations


#pragma region Left_Limbs

void UCharacter_Animation_Instance::Set_Left_Hand_Shimmy_Location_Implementation(const FVector& New_Left_Hand_Shimmy_Location)
{   
    const double Delta_Time{UGameplayStatics::GetWorldDeltaSeconds(this)};

    double Left_Hand_Shimmy_Location_Interpolation_Speed{};
    
    if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))) || 
       Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Wall.Pipe.Climb"))))
    Left_Hand_Shimmy_Location_Interpolation_Speed = 15.f;

    else
    Left_Hand_Shimmy_Location_Interpolation_Speed = 700.f;
    
    const FVector Left_Hand_Shimmy_Location_Interpolated{UKismetMathLibrary::VInterpTo(Left_Hand_Shimmy_Location,
                                                                                      New_Left_Hand_Shimmy_Location,
                                                                                      Delta_Time,
                                                                                      Left_Hand_Shimmy_Location_Interpolation_Speed
                                                                                      )};
    
    Left_Hand_Shimmy_Location = Left_Hand_Shimmy_Location_Interpolated;
}

void UCharacter_Animation_Instance::Set_Left_Hand_Shimmy_Rotation_Implementation(const FRotator& New_Left_Hand_Shimmy_Rotation)
{
    const double Delta_Time{UGameplayStatics::GetWorldDeltaSeconds(this)};

    double Left_Hand_Shimmy_Rotation_Interpolation_Speed{};

    if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))) || 
       Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Wall.Pipe.Climb"))))
    Left_Hand_Shimmy_Rotation_Interpolation_Speed = 15.f;
    
    else
    Left_Hand_Shimmy_Rotation_Interpolation_Speed = 700.f;

    const FRotator Left_Hand_Shimmy_Rotation_Interpolated{UKismetMathLibrary::RInterpTo(Left_Hand_Shimmy_Rotation,
                                                                                        New_Left_Hand_Shimmy_Rotation,
                                                                                        Delta_Time,
                                                                                        Left_Hand_Shimmy_Rotation_Interpolation_Speed)};

    Left_Hand_Shimmy_Rotation = Left_Hand_Shimmy_Rotation_Interpolated;

}

void UCharacter_Animation_Instance::Set_Left_Foot_Shimmy_Location_Implementation(const FVector& New_Left_Foot_Shimmy_Location)
{
    const double Delta_Time{UGameplayStatics::GetWorldDeltaSeconds(this)};

    double Left_Foot_Shimmy_Location_Interpolation_Speed{};
    
    if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))) || 
       Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Wall.Pipe.Climb"))))
    Left_Foot_Shimmy_Location_Interpolation_Speed = 35.f;

    else
    Left_Foot_Shimmy_Location_Interpolation_Speed = 700.f;
    
    const FVector Left_Foot_Shimmy_Location_Interpolated{UKismetMathLibrary::VInterpTo(Left_Foot_Shimmy_Location,
                                                                                      New_Left_Foot_Shimmy_Location,
                                                                                      Delta_Time,
                                                                                      Left_Foot_Shimmy_Location_Interpolation_Speed
                                                                                      )};
    
    Left_Foot_Shimmy_Location = Left_Foot_Shimmy_Location_Interpolated;
}

void UCharacter_Animation_Instance::Set_Left_Foot_Shimmy_Rotation_Implementation(const FRotator& New_Left_Foot_Shimmy_Rotation)
{
    const double Delta_Time{UGameplayStatics::GetWorldDeltaSeconds(this)};

    double Left_Foot_Shimmy_Rotation_Interpolation_Speed{};

    if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))) || 
       Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Wall.Pipe.Climb"))))
    Left_Foot_Shimmy_Rotation_Interpolation_Speed = 30.f;
    
    else
    Left_Foot_Shimmy_Rotation_Interpolation_Speed = 700.f;

    const FRotator Left_Foot_Shimmy_Rotation_Interpolated{UKismetMathLibrary::RInterpTo(Left_Foot_Shimmy_Rotation,
                                                                                        New_Left_Foot_Shimmy_Rotation,
                                                                                        Delta_Time,
                                                                                        Left_Foot_Shimmy_Rotation_Interpolation_Speed)};

    Left_Foot_Shimmy_Rotation = Left_Foot_Shimmy_Rotation_Interpolated;
}

#pragma endregion

#pragma region Right_Limbs

void UCharacter_Animation_Instance::Set_Right_Hand_Shimmy_Location_Implementation(const FVector& New_Right_Hand_Shimmy_Location)
{
    const double Delta_Time{UGameplayStatics::GetWorldDeltaSeconds(this)};

    double Right_Hand_Shimmy_Location_Interpolation_Speed{};
    
    if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))) || 
       Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Wall.Pipe.Climb"))))
    Right_Hand_Shimmy_Location_Interpolation_Speed = 15.f;

    else
    Right_Hand_Shimmy_Location_Interpolation_Speed = 700.f;
    
    const FVector Right_Hand_Shimmy_Location_Interpolated{UKismetMathLibrary::VInterpTo(Right_Hand_Shimmy_Location,
                                                                                       New_Right_Hand_Shimmy_Location,
                                                                                       Delta_Time,
                                                                                       Right_Hand_Shimmy_Location_Interpolation_Speed
                                                                                       )};
    
    Right_Hand_Shimmy_Location = Right_Hand_Shimmy_Location_Interpolated;
}

void UCharacter_Animation_Instance::Set_Right_Hand_Shimmy_Rotation_Implementation(const FRotator& New_Right_Hand_Shimmy_Rotation)
{
    const double Delta_Time{UGameplayStatics::GetWorldDeltaSeconds(this)};

    double Right_Hand_Shimmy_Rotation_Interpolation_Speed{};

   if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))) || 
       Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Wall.Pipe.Climb"))))
    Right_Hand_Shimmy_Rotation_Interpolation_Speed = 15.f;
    
    else
    Right_Hand_Shimmy_Rotation_Interpolation_Speed = 700.f;

    const FRotator Right_Hand_Shimmy_Rotation_Interpolated{UKismetMathLibrary::RInterpTo(Right_Hand_Shimmy_Rotation,
                                                                                        New_Right_Hand_Shimmy_Rotation,
                                                                                        Delta_Time,
                                                                                        Right_Hand_Shimmy_Rotation_Interpolation_Speed)};

    Right_Hand_Shimmy_Rotation = Right_Hand_Shimmy_Rotation_Interpolated;
}

void UCharacter_Animation_Instance::Set_Right_Foot_Shimmy_Location_Implementation(const FVector& New_Right_Foot_Shimmy_Location)
{
    const double Delta_Time{UGameplayStatics::GetWorldDeltaSeconds(this)};

    double Right_Foot_Shimmy_Location_Interpolation_Speed{};
    
    if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))) || 
       Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Wall.Pipe.Climb"))))
    Right_Foot_Shimmy_Location_Interpolation_Speed = 35.f;

    else
    Right_Foot_Shimmy_Location_Interpolation_Speed = 700.f;
    
    const FVector Right_Foot_Shimmy_Location_Interpolated{UKismetMathLibrary::VInterpTo(Right_Foot_Shimmy_Location,
                                                                                       New_Right_Foot_Shimmy_Location,
                                                                                       Delta_Time,
                                                                                       Right_Foot_Shimmy_Location_Interpolation_Speed
                                                                                       )};
    
    Right_Foot_Shimmy_Location = Right_Foot_Shimmy_Location_Interpolated;
}

void UCharacter_Animation_Instance::Set_Right_Foot_Shimmy_Rotation_Implementation(const FRotator& New_Right_Foot_Shimmy_Rotation)
{
    const double Delta_Time{UGameplayStatics::GetWorldDeltaSeconds(this)};

    double Right_Foot_Shimmy_Rotation_Interpolation_Speed{};

    if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))) || 
       Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Wall.Pipe.Climb"))))
    Right_Foot_Shimmy_Rotation_Interpolation_Speed = 15.f;
    
    else
    Right_Foot_Shimmy_Rotation_Interpolation_Speed = 700.f;

    const FRotator Right_Foot_Shimmy_Rotation_Interpolated{UKismetMathLibrary::RInterpTo(Right_Foot_Shimmy_Rotation,
                                                                                        New_Right_Foot_Shimmy_Rotation,
                                                                                        Delta_Time,
                                                                                        Right_Foot_Shimmy_Rotation_Interpolation_Speed)};

    Right_Foot_Shimmy_Rotation = Right_Foot_Shimmy_Rotation_Interpolated;
}

#pragma endregion


#pragma endregion

#pragma endregion

