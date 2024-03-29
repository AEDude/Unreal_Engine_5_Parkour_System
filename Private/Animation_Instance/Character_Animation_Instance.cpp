// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation_Instance/Character_Animation_Instance.h"
#include "Character/Technical_Animator_Character.h"
#include "Components/Custom_Movement_Component.h"
#include "Kismet/KismetMathLibrary.h"
#include "Debug/DebugHelper.h"
#include "Kismet/GameplayStatics.h"


void UCharacter_Animation_Instance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    Climbing_System_Character = Cast<ATechnical_Animator_Character>(TryGetPawnOwner());

    if(Climbing_System_Character)
    {
        
        Custom_Movement_Component = Climbing_System_Character->Get_Custom_Movement_Component();
    }
}

void UCharacter_Animation_Instance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if(!Climbing_System_Character || !Custom_Movement_Component) return;

    Get_Ground_Speed();
    Get_Air_Speed();
    Get_Should_Move();
    Get_Is_Falling();
    Get_Is_Climbing();
    Get_Climb_Velocity();
    Get_Is_Taking_Cover();
    Get_Take_Cover_Velocity();

    Forward_Backward_Movement_Value = Custom_Movement_Component->Forward_Backward_Movement_Value;

    Right_Left_Movement_Value = Custom_Movement_Component->Right_Left_Movement_Value;

}

void UCharacter_Animation_Instance::Get_Ground_Speed()
{
   Ground_Speed = UKismetMathLibrary::VSizeXY(Climbing_System_Character->GetVelocity());
}

void UCharacter_Animation_Instance::Get_Air_Speed()
{
   Air_Speed = Climbing_System_Character->GetVelocity().Z;
}

void UCharacter_Animation_Instance::Get_Should_Move()
{
    bShould_Move =
    Custom_Movement_Component->GetCurrentAcceleration().Size()>0 && 
    Ground_Speed>5.f &&
    !bIs_Falling;
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

void UCharacter_Animation_Instance::Set_Climb_Style_Implementation(const FGameplayTag &New_Climb_Style)
{
   Parkour_Climb_Style = New_Climb_Style;
}

void UCharacter_Animation_Instance::Set_Climb_Direction_Implementation(const FGameplayTag &New_Climb_Direction)
{
    Parkour_Direction = New_Climb_Direction;

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
            

    if(Parkour_Direction == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Right"))))
    Debug::Print("Parkour_Direction_Right", FColor::MakeRandomColor(), 1);

    else if((Parkour_Direction == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.Left")))))
    Debug::Print("Parkour_Direction_Left", FColor::MakeRandomColor(), 1);

    if(Parkour_Direction == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.Direction.None"))))
    Debug::Print("Parkour_Direction_None", FColor::MakeRandomColor(), 1);
}

#pragma endregion


#pragma region Limbs_Location_And_Rotations

#pragma region Left_Limbs

void UCharacter_Animation_Instance::Set_Left_Hand_Shimmy_Location_Implementation(const FVector& New_Left_Hand_Shimmy_Location)
{   
    const double Delta_Time{UGameplayStatics::GetWorldDeltaSeconds(this)};

    double Left_Hand_Shimmy_Location_Interpolation_Speed{};
    
    if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))))
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

    if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))))
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
    
    if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))))
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

    if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))))
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
    
    if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))))
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

    if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))))
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
    
    if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))))
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

    if(Parkour_State == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Climb"))))
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

