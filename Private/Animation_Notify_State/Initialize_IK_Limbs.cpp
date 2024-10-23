// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation_Notify_State/Initialize_IK_Limbs.h"
#include "Components/Custom_Movement_Component.h"


UInitialize_IK_Limbs::UInitialize_IK_Limbs()
{
}

void UInitialize_IK_Limbs::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
   /*The object of this class is created within animation montages in the form of animation noifies. The notifies trigger and the function 
   "NotifyEnd(this function) will be called. Within this function the "MeshComp" pointer is used to access the the owner of the animation montage, 
   followed by getting the UCustom_Movement_Component by class. Finally the functions within said component which handle executing the ray traces 
   that determine where the feet and hands of the character will be placed when the character initially begins shimmying are called. Within these 
   functions the Parkour_Locomotion_Interface is called setting the location of the hands and feet by passing in FVector and FRotator values as const&. 
   The functions within the "UCustom_Movement_Component" which handle the execution of determining the location to place the hands and feet of the 
   character are private, therefore public FORCEINLINE setter functions are decalred within the "UCustom_Movement_Component" and called within this class.*/

    if(MeshComp->GetOwner()->GetComponentByClass<UCustom_Movement_Component>()->Get_Parkour_State() == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Ready.To.Climb"))))
    {
       Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
       
        MeshComp->GetOwner()->GetComponentByClass<UCustom_Movement_Component>()->Set_Parkour_Climb_Initialize_IK_Hands(true);

        MeshComp->GetOwner()->GetComponentByClass<UCustom_Movement_Component>()->Set_Parkour_Climb_Initialize_IK_Hands(false);

        MeshComp->GetOwner()->GetComponentByClass<UCustom_Movement_Component>()->Set_Parkour_Climb_Initialize_IK_Feet(true);

        MeshComp->GetOwner()->GetComponentByClass<UCustom_Movement_Component>()->Set_Parkour_Climb_Initialize_IK_Feet(false); 
    }

    else if(MeshComp->GetOwner()->GetComponentByClass<UCustom_Movement_Component>()->Get_Parkour_State() == FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Initialize.Wall.Pipe.Climb"))))
    {
        Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
        
        MeshComp->GetOwner()->GetComponentByClass<UCustom_Movement_Component>()->Set_Parkour_Wall_Pipe_Climb_Initialize_IK_Hands(true);

        MeshComp->GetOwner()->GetComponentByClass<UCustom_Movement_Component>()->Set_Parkour_Wall_Pipe_Climb_Initialize_IK_Hands(false);

        MeshComp->GetOwner()->GetComponentByClass<UCustom_Movement_Component>()->Set_Parkour_Wall_Pipe_Climb_Initialize_IK_Feet(true);

        MeshComp->GetOwner()->GetComponentByClass<UCustom_Movement_Component>()->Set_Parkour_Wall_Pipe_Climb_Initialize_IK_Feet(false);
    }
    
}