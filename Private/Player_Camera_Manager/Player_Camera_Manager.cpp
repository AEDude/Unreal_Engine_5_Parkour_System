// Fill out your copyright notice in the Description page of Project Settings.


#include "Player_Camera_Manager/Player_Camera_Manager.h"
#include "Character/Technical_Animator_Character.h"
#include "Debug/DebugHelper.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"


void APlayer_Camera_Manager::UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime)
{
    OutVT.SetNewTarget(Cast<ATechnical_Animator_Character>(GetOwningPlayerController()->GetPawn()));

    //Get the location of the owner.
    const FVector Owner_Location{OutVT.Target->GetActorLocation()};

    //Assign the controller rotation to the global FRotator "New_Camera_Rotation".
    New_Camera_Rotation = GetOwningPlayerController()->GetControlRotation();

    //Get the forward vector of the global FRotator "New_Camera_Rotation".
    const FVector Controller_Forward_Vector{UKismetMathLibrary::GetForwardVector(New_Camera_Rotation)};

    //Get the forward vector of the global FRotator "New_Camera_Rotation".
    const FVector ControllerRight_Vector{UKismetMathLibrary::GetRightVector(New_Camera_Rotation)};
            
    //Offset the location of the camera upwards
    const FVector Offset_1{Owner_Location + FVector(0, 0, 30.f)};

    //Offset the location of the camera backwards so that it is behind the character.
    const FVector Offset_2{Offset_1 + (Controller_Forward_Vector * -220)};

    //Offset the location of the camera to the right or left for a more aesthetic view.
    New_Camera_Location = (Offset_2 + (ControllerRight_Vector * -70));

    //Set the location, rotation and FOV of the camera.
    OutVT.POV.Location = New_Camera_Location;

    OutVT.POV.Rotation = New_Camera_Rotation;

    OutVT.POV.FOV = New_Camera_FOV;
}