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

        if(Custom_Movement_Component)
        {
            
        }

        /* Primary_Rotation = Technical_Animator_Character->GetActorRotation(); */
    }  
}

void UCharacter_Animation_Instance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

    Update_Variables_On_Secondary_Thread(DeltaSeconds);
}

void UCharacter_Animation_Instance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    Get_Is_Falling();
    
}

void UCharacter_Animation_Instance::NativePostEvaluateAnimation()
{
    Super::NativePostEvaluateAnimation();

   /*  if((Character_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Free.Roam"))) || 
	Character_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Stairs")))) &&
    Character_Action == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.Action.No.Action"))))
    {
      Update_Character_Rotation();
    }  */
}

void UCharacter_Animation_Instance::Update_Variables_On_Secondary_Thread(const float& DeltaSeconds)
{
    if(!Technical_Animator_Character || !Custom_Movement_Component)
    {
        return;
    }
    
    Get_Is_Crouching();
    
    Get_Acceleration();

    Get_Ground_Speed();

    Get_Air_Speed();

    Get_Velocity();

    Calculate_Direction();

    Get_Predicted_Stop_Distance_Variables();

    

    if(Custom_Movement_Component)
    {
        Forward_Backward_Movement_Value = Custom_Movement_Component->Get_Forward_Backward_Movement_Value();

        Right_Left_Movement_Value = Custom_Movement_Component->Get_Right_Left_Movement_Value();

        bCan_Initialize_Running_Start = Custom_Movement_Component->Get_bCan_Initialize_Running_Start();
    }

    //Get_Input_Vector();

    //Get_Is_Jogging();

    //Get_Should_Move();

    // Get_Is_Climbing();

    // Get_Climb_Velocity();

    // Get_Is_Taking_Cover();

    // Get_Take_Cover_Velocity();
}


#pragma region Custom_Locomoton

#pragma region Custom_Locomotion_Helper

void UCharacter_Animation_Instance::Get_Is_Crouching()
{
    if(Technical_Animator_Character)
    {
        bIs_Crouching = Technical_Animator_Character->bIsCrouched;
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
   if(Custom_Movement_Component)
   {
    //This is the velocity of the characters movement on the global X and Y axis. The Air velocity (Z axis) has been omitted.
    Ground_Speed = UKismetMathLibrary::VSizeXY(Custom_Movement_Component->Velocity);
   }
}

void UCharacter_Animation_Instance::Get_Air_Speed()
{
   if(Custom_Movement_Component)
   {
        Air_Speed = Custom_Movement_Component->Velocity.Z;
   }
   
}

void UCharacter_Animation_Instance::Get_Velocity()
{
    if(Custom_Movement_Component)
    {
       //The velocity is a vector that points in the direction in which the character is moving
        Velocity = Custom_Movement_Component->Velocity;

        Velocity = Velocity.GetSafeNormal(); 
    } 
}

void UCharacter_Animation_Instance::Calculate_Direction()
{
    if(Technical_Animator_Character)
    {
       Velocity_Locomotion_Angle = UKismetAnimationLibrary::CalculateDirection(Velocity, Technical_Animator_Character->GetActorRotation()); 
    }
}

void UCharacter_Animation_Instance::Get_Is_Falling()
{
    if(Custom_Movement_Component)
    {
        bIs_Falling = Custom_Movement_Component->IsFalling();
    }
}

/* 
void UCharacter_Animation_Instance::Get_Input_Vector()
{
   if(Custom_Movement_Component)
   {
        Input_Vector = Custom_Movement_Component->GetLastInputVector();
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
*/

#pragma endregion

#pragma region Custom_Ground_Locomotion_Core

/* void UCharacter_Animation_Instance::Update_Character_Rotation()
{
    if(Ground_Locomotion_State == EGround_Locomotion_State::EGLS_Idle)
    {
        Update_Rotation_Turn_In_Place();
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

} */

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
    
    if(Ground_Locomotion_State == EGround_Locomotion_State::EGLS_Idle)
    {
        Distance_To_Match = UAnimCharacterMovementLibrary::PredictGroundMovementStopLocation(Velocity, Custom_Movement_Component->bUseSeparateBrakingFriction, Custom_Movement_Component->BrakingFriction, 
                                                                                             Custom_Movement_Component->GroundFriction, Custom_Movement_Component->BrakingFrictionFactor, 
                                                                                             Custom_Movement_Component->BrakingDecelerationWalking).Length();
    }

    else
    {
        Distance_To_Match = 0.f;
    }
}

#pragma endregion

#pragma endregion

#pragma region Character_Interface

#pragma region Character_Locomotion

void UCharacter_Animation_Instance::Set_Parkour_State_Implementation(const FGameplayTag &New_Character_State)
{
   Character_State = New_Character_State;
}

void UCharacter_Animation_Instance::Set_Parkour_Action_Implementation(const FGameplayTag &New_Character_Action)
{
    Character_Action = New_Character_Action;
}

void UCharacter_Animation_Instance::Set_Parkour_Climb_Style_Implementation(const FGameplayTag &New_Climb_Style)
{
   Character_Climb_Style = New_Climb_Style;
}

void UCharacter_Animation_Instance::Set_Parkour_Wall_Run_Side_Implementation(const FGameplayTag& New_Wall_Run_Side)
{
    Character_Wall_Run_Side = New_Wall_Run_Side;
}

void UCharacter_Animation_Instance::Set_Parkour_Direction_Implementation(const FGameplayTag& New_Character_Direction)
{
    Character_Direction = New_Character_Direction;

    if(Character_Direction == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.Direction.Forward"))) || 
       Character_Direction == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.Direction.Forward.Right"))) ||
       Character_Direction == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.Direction.Right"))))
    {
        Left_Hand_Curve_Alpha = 1.f;
        Right_Hand_Curve_Alpha = 0.f;
    }
    
    else if((Character_Direction == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.Direction.Left")))) ||
            (Character_Direction == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.Direction.Forward.Left")))))
    {
        Left_Hand_Curve_Alpha = 0.f;
        Right_Hand_Curve_Alpha = 1.f;
    }

    else if(Character_Direction == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.Direction.Backward"))) ||
            Character_Direction == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.Direction.Backward.Left"))) ||
            Character_Direction == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.Direction.Backward.Right"))))   
    {
        Left_Hand_Curve_Alpha = 1.f;
        Right_Hand_Curve_Alpha = 1.f;
    }
            

    Debug::Print("Character_Direction: " + Character_Direction.ToString(), FColor::MakeRandomColor(), 2000);
}

void UCharacter_Animation_Instance::Set_Parkour_Stairs_Direction_Implementation(const FGameplayTag& New_Character_Stairs_Direction)
{
    Character_Stairs_Direction = New_Character_Stairs_Direction;
    
}

void UCharacter_Animation_Instance::Set_Parkour_Slide_Side_Implementation(const FGameplayTag& New_Character_Slide_Side)
{
    Character_Slide_Side = New_Character_Slide_Side;
}

#pragma endregion

#pragma region Limbs_Location_And_Rotations


#pragma region Left_Limbs

void UCharacter_Animation_Instance::Set_Left_Hand_Shimmy_Location_Implementation(const FVector& New_Left_Hand_Shimmy_Location)
{   
    const double Delta_Time{UGameplayStatics::GetWorldDeltaSeconds(this)};

    double Left_Hand_Shimmy_Location_Interpolation_Speed{};
    
    if(Character_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Climb"))) || 
       Character_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Wall.Pipe.Climb"))))
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

    if(Character_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Climb"))) || 
       Character_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Wall.Pipe.Climb"))))
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
    
    if(Character_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Climb"))) || 
       Character_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Wall.Pipe.Climb"))))
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

    if(Character_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Climb"))) || 
       Character_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Wall.Pipe.Climb"))))
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
    
    if(Character_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Climb"))) || 
       Character_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Wall.Pipe.Climb"))))
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

   if(Character_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Climb"))) || 
       Character_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Wall.Pipe.Climb"))))
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
    
    if(Character_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Climb"))) || 
       Character_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Wall.Pipe.Climb"))))
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

    if(Character_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Climb"))) || 
       Character_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Wall.Pipe.Climb"))))
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

