// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation_Notify_State/Foot_Steps.h"
#include "Character/Technical_Animator_Character.h"
#include "Components/Custom_Movement_Component.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Debug/DebugHelper.h"

void UFoot_Steps::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
    
    if(MeshComp)
    {
        AActor* Mesh_Owner{MeshComp->GetOwner()};

        if(Mesh_Owner)
        {
            ATechnical_Animator_Character* Technical_Animator_Character = Cast<ATechnical_Animator_Character>(Mesh_Owner);
               
            if(Technical_Animator_Character && Technical_Animator_Character->IsLocallyControlled())
            {
                UCustom_Movement_Component* Custom_Movement_Component = Technical_Animator_Character->Get_Custom_Movement_Component();

                if(Custom_Movement_Component)
                {
	                TArray<TEnumAsByte<EObjectTypeQuery>> Object_Trace_Types{};
                    
                    Object_Trace_Types.Emplace(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
                    Object_Trace_Types.Emplace(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
                    Object_Trace_Types.Emplace(UEngineTypes::ConvertToObjectType(ECC_Vehicle));
                    Object_Trace_Types.Emplace(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));

                   
                    //Get the location of both feet bones
                    const FVector Left_Foot_Location{Technical_Animator_Character->GetMesh()->GetSocketLocation(FName(TEXT("foot_l")))};

                    const FVector Right_Foot_Location{Technical_Animator_Character->GetMesh()->GetSocketLocation(FName(TEXT("foot_r")))};


                   
                   
                    /* Perform line traces on both feet to get information on the ground. */

                    //Left Foot 

                    const FVector Offset_Left_Foot_Location_Up{Move_Vector_Up(Custom_Movement_Component, Left_Foot_Location, 20.f)};

                    const FVector Start_Left_Foot{Offset_Left_Foot_Location_Up};

                    const FVector Offset_Left_Foot_Location_Down{Move_Vector_Down(Custom_Movement_Component, Start_Left_Foot, 30.f)};

                    const FVector End_Left_Foot{Offset_Left_Foot_Location_Down};

                    FHitResult Left_Foot_Out_Hit{};

                    UKismetSystemLibrary::SphereTraceSingleForObjects(
                        Custom_Movement_Component,
                        Start_Left_Foot,
                        End_Left_Foot,
                        5.f,
                        Object_Trace_Types,
                        false,
                        TArray<AActor*>(),
                        EDrawDebugTrace::None,
                        Left_Foot_Out_Hit,
                        false
                    );

                    if(Left_Foot_Out_Hit.bBlockingHit)
                    {
                        EPhysicalSurface Surface_Type{};
                        
                        Surface_Type = UGameplayStatics::GetSurfaceType(Left_Foot_Out_Hit);

                        switch(Surface_Type)
                        {
                            case EPhysicalSurface::SurfaceType1:

                            if(Custom_Movement_Component->Get_Concrete_Foot_Step_Sound())
                            {
                                //Only play the foot step sound here if this call is not from the server. If the server is the one making this call its foot steps will be played locally within 
                                //&UCustom_Movement_Component::Server_Play_Foot_Steps_Implementation.
                                if(!Technical_Animator_Character->HasAuthority())
                                {
                                   UGameplayStatics::PlaySoundAtLocation(
                                        Custom_Movement_Component,
                                        Custom_Movement_Component->Get_Concrete_Foot_Step_Sound(),
                                        Left_Foot_Out_Hit.ImpactPoint
                                    ); 
                                }
                                
                                Custom_Movement_Component->Server_Play_Foot_Steps(Custom_Movement_Component->Get_Concrete_Foot_Step_Sound(), Left_Foot_Out_Hit.ImpactPoint); 
                            }
                            
                            break;

                            case EPhysicalSurface::SurfaceType2:

                            if(Custom_Movement_Component->Get_Creaky_Wood_Foot_Step_Sound())
                            {
                                //Only play the foot step sound here if this call is not from the server. If the server is the one making this call its foot steps will be played locally within 
                                //&UCustom_Movement_Component::Server_Play_Foot_Steps_Implementation.
                                if(!Technical_Animator_Character->HasAuthority())
                                {
                                    UGameplayStatics::PlaySoundAtLocation(
                                        Custom_Movement_Component,
                                        Custom_Movement_Component->Get_Creaky_Wood_Foot_Step_Sound(),
                                        Left_Foot_Out_Hit.ImpactPoint
                                    );
                                }
                                
                                Custom_Movement_Component->Server_Play_Foot_Steps(Custom_Movement_Component->Get_Creaky_Wood_Foot_Step_Sound(), Left_Foot_Out_Hit.ImpactPoint); 
                            }
                            
                            break;
                            
                            case EPhysicalSurface::SurfaceType3:

                            if(Custom_Movement_Component->Get_Dirt_Foot_Step_Sound())
                            {
                                //Only play the foot step sound here if this call is not from the server. If the server is the one making this call its foot steps will be played locally within 
                                //&UCustom_Movement_Component::Server_Play_Foot_Steps_Implementation.
                                if(!Technical_Animator_Character->HasAuthority())
                                {
                                    UGameplayStatics::PlaySoundAtLocation(
                                        Custom_Movement_Component,
                                        Custom_Movement_Component->Get_Dirt_Foot_Step_Sound(),
                                        Left_Foot_Out_Hit.ImpactPoint
                                    );
                                }

                                Custom_Movement_Component->Server_Play_Foot_Steps(Custom_Movement_Component->Get_Dirt_Foot_Step_Sound(), Left_Foot_Out_Hit.ImpactPoint); 
                            }
                            
                            break;

                            case EPhysicalSurface::SurfaceType4:

                            if(Custom_Movement_Component->Get_Grass_Foot_Step_Sound())
                            {
                                //Only play the foot step sound here if this call is not from the server. If the server is the one making this call its foot steps will be played locally within 
                                //&UCustom_Movement_Component::Server_Play_Foot_Steps_Implementation.
                                if(!Technical_Animator_Character->HasAuthority())
                                {
                                    UGameplayStatics::PlaySoundAtLocation(
                                        Custom_Movement_Component,
                                        Custom_Movement_Component->Get_Grass_Foot_Step_Sound(),
                                        Left_Foot_Out_Hit.ImpactPoint
                                    );
                                }

                                Custom_Movement_Component->Server_Play_Foot_Steps(Custom_Movement_Component->Get_Grass_Foot_Step_Sound(), Left_Foot_Out_Hit.ImpactPoint); 
                            }
                            
                            break;

                            case EPhysicalSurface::SurfaceType5:

                            if(Custom_Movement_Component->Get_Gravel_Foot_Step_Sound())
                            {
                                //Only play the foot step sound here if this call is not from the server. If the server is the one making this call its foot steps will be played locally within 
                                //&UCustom_Movement_Component::Server_Play_Foot_Steps_Implementation.
                                if(!Technical_Animator_Character->HasAuthority())
                                {
                                    UGameplayStatics::PlaySoundAtLocation(
                                        Custom_Movement_Component,
                                        Custom_Movement_Component->Get_Gravel_Foot_Step_Sound(),
                                        Left_Foot_Out_Hit.ImpactPoint
                                    );
                                }

                                Custom_Movement_Component->Server_Play_Foot_Steps(Custom_Movement_Component->Get_Gravel_Foot_Step_Sound(), Left_Foot_Out_Hit.ImpactPoint);  
                            }
                            
                            break;

                            case EPhysicalSurface::SurfaceType6:

                            if(Custom_Movement_Component->Get_Heel_Foot_Step_Sound())
                            {
                                //Only play the foot step sound here if this call is not from the server. If the server is the one making this call its foot steps will be played locally within 
                                //&UCustom_Movement_Component::Server_Play_Foot_Steps_Implementation.
                                if(!Technical_Animator_Character->HasAuthority())
                                {
                                    UGameplayStatics::PlaySoundAtLocation(
                                        Custom_Movement_Component,
                                        Custom_Movement_Component->Get_Heel_Foot_Step_Sound(),
                                        Left_Foot_Out_Hit.ImpactPoint
                                    );
                                }

                                Custom_Movement_Component->Server_Play_Foot_Steps(Custom_Movement_Component->Get_Heel_Foot_Step_Sound(), Left_Foot_Out_Hit.ImpactPoint);   
                            }
                            
                            break;

                            case EPhysicalSurface::SurfaceType7:

                            if(Custom_Movement_Component->Get_Leaves_Foot_Step_Sound())
                            {
                                //Only play the foot step sound here if this call is not from the server. If the server is the one making this call its foot steps will be played locally within 
                                //&UCustom_Movement_Component::Server_Play_Foot_Steps_Implementation.
                                if(!Technical_Animator_Character->HasAuthority())
                                {
                                    UGameplayStatics::PlaySoundAtLocation(
                                        Custom_Movement_Component,
                                        Custom_Movement_Component->Get_Leaves_Foot_Step_Sound(),
                                        Left_Foot_Out_Hit.ImpactPoint
                                    );
                                }

                                Custom_Movement_Component->Server_Play_Foot_Steps(Custom_Movement_Component->Get_Leaves_Foot_Step_Sound(), Left_Foot_Out_Hit.ImpactPoint); 
                            }
                            
                            break;

                            case EPhysicalSurface::SurfaceType8:

                            if(Custom_Movement_Component->Get_Low_Stone_Foot_Step_Sound())
                            {
                                //Only play the foot step sound here if this call is not from the server. If the server is the one making this call its foot steps will be played locally within 
                                //&UCustom_Movement_Component::Server_Play_Foot_Steps_Implementation.
                                if(!Technical_Animator_Character->HasAuthority())
                                {
                                    UGameplayStatics::PlaySoundAtLocation(
                                        Custom_Movement_Component,
                                        Custom_Movement_Component->Get_Low_Stone_Foot_Step_Sound(),
                                        Left_Foot_Out_Hit.ImpactPoint
                                    );
                                }
                 
                                Custom_Movement_Component->Server_Play_Foot_Steps(Custom_Movement_Component->Get_Low_Stone_Foot_Step_Sound(), Left_Foot_Out_Hit.ImpactPoint);
                            }
                            
                            break;

                            case EPhysicalSurface::SurfaceType9:

                            if(Custom_Movement_Component->Get_Water_Foot_Step_Sound())
                            {
                                //Only play the foot step sound here if this call is not from the server. If the server is the one making this call its foot steps will be played locally within 
                                //&UCustom_Movement_Component::Server_Play_Foot_Steps_Implementation.
                                if(!Technical_Animator_Character->HasAuthority())
                                {
                                   UGameplayStatics::PlaySoundAtLocation(
                                        Custom_Movement_Component,
                                        Custom_Movement_Component->Get_Water_Foot_Step_Sound(),
                                        Left_Foot_Out_Hit.ImpactPoint
                                    ); 
                                }

                                Custom_Movement_Component->Server_Play_Foot_Steps(Custom_Movement_Component->Get_Water_Foot_Step_Sound(), Left_Foot_Out_Hit.ImpactPoint);
                            }
                            
                            break;

                            default:
                            Debug::Print("Error Selecting Foot Step Sound", FColor::Red, 1000);
                        }
                    }

                    //Right Foot 

                    const FVector Offset_Right_Foot_Location_Up{Move_Vector_Up(Custom_Movement_Component, Right_Foot_Location, 30.f)};

                    const FVector Start_Right_Foot{Offset_Right_Foot_Location_Up};

                    const FVector Offset_Right_Foot_Location_Down{Move_Vector_Down(Custom_Movement_Component, Start_Right_Foot, 50.f)};

                    const FVector End_Right_Foot{Offset_Right_Foot_Location_Down};

                    
                    FHitResult Right_Foot_Out_Hit{};

                    UKismetSystemLibrary::SphereTraceSingleForObjects(
                        Custom_Movement_Component,
                        Start_Right_Foot,
                        End_Right_Foot,
                        5.f,
                        Object_Trace_Types,
                        false,
                        TArray<AActor*>(),
                        EDrawDebugTrace::None,
                        Right_Foot_Out_Hit,
                        false
                    );

                    if(Right_Foot_Out_Hit.bBlockingHit)
                    {
                        EPhysicalSurface Surface_Type{};
                        
                        Surface_Type = UGameplayStatics::GetSurfaceType(Right_Foot_Out_Hit);

                        switch(Surface_Type)
                        {
                            case EPhysicalSurface::SurfaceType1:

                            if(Custom_Movement_Component->Get_Concrete_Foot_Step_Sound())
                            {
                                //Only play the foot step sound here if this call is not from the server. If the server is the one making this call its foot steps will be played locally within 
                                //&UCustom_Movement_Component::Server_Play_Foot_Steps_Implementation.
                                if(!Technical_Animator_Character->HasAuthority())
                                {
                                   UGameplayStatics::PlaySoundAtLocation(
                                        Custom_Movement_Component,
                                        Custom_Movement_Component->Get_Concrete_Foot_Step_Sound(),
                                        Right_Foot_Out_Hit.ImpactPoint
                                    ); 
                                }
                                
                                Custom_Movement_Component->Server_Play_Foot_Steps(Custom_Movement_Component->Get_Concrete_Foot_Step_Sound(), Right_Foot_Out_Hit.ImpactPoint); 
                            }
                            
                            break;

                            case EPhysicalSurface::SurfaceType2:

                            if(Custom_Movement_Component->Get_Creaky_Wood_Foot_Step_Sound())
                            {
                                //Only play the foot step sound here if this call is not from the server. If the server is the one making this call its foot steps will be played locally within 
                                //&UCustom_Movement_Component::Server_Play_Foot_Steps_Implementation.
                                if(!Technical_Animator_Character->HasAuthority())
                                {
                                    UGameplayStatics::PlaySoundAtLocation(
                                        Custom_Movement_Component,
                                        Custom_Movement_Component->Get_Creaky_Wood_Foot_Step_Sound(),
                                        Right_Foot_Out_Hit.ImpactPoint
                                    );
                                }
                                
                                Custom_Movement_Component->Server_Play_Foot_Steps(Custom_Movement_Component->Get_Creaky_Wood_Foot_Step_Sound(), Right_Foot_Out_Hit.ImpactPoint); 
                            }
                            
                            break;
                            
                            case EPhysicalSurface::SurfaceType3:

                            if(Custom_Movement_Component->Get_Dirt_Foot_Step_Sound())
                            {
                                //Only play the foot step sound here if this call is not from the server. If the server is the one making this call its foot steps will be played locally within 
                                //&UCustom_Movement_Component::Server_Play_Foot_Steps_Implementation.
                                if(!Technical_Animator_Character->HasAuthority())
                                {
                                    UGameplayStatics::PlaySoundAtLocation(
                                        Custom_Movement_Component,
                                        Custom_Movement_Component->Get_Dirt_Foot_Step_Sound(),
                                        Right_Foot_Out_Hit.ImpactPoint
                                    );
                                }

                                Custom_Movement_Component->Server_Play_Foot_Steps(Custom_Movement_Component->Get_Dirt_Foot_Step_Sound(), Right_Foot_Out_Hit.ImpactPoint); 
                            }
                            
                            break;

                            case EPhysicalSurface::SurfaceType4:

                            if(Custom_Movement_Component->Get_Grass_Foot_Step_Sound())
                            {
                                //Only play the foot step sound here if this call is not from the server. If the server is the one making this call its foot steps will be played locally within 
                                //&UCustom_Movement_Component::Server_Play_Foot_Steps_Implementation.
                                if(!Technical_Animator_Character->HasAuthority())
                                {
                                    UGameplayStatics::PlaySoundAtLocation(
                                        Custom_Movement_Component,
                                        Custom_Movement_Component->Get_Grass_Foot_Step_Sound(),
                                        Right_Foot_Out_Hit.ImpactPoint
                                    );
                                }

                                Custom_Movement_Component->Server_Play_Foot_Steps(Custom_Movement_Component->Get_Grass_Foot_Step_Sound(), Right_Foot_Out_Hit.ImpactPoint); 
                            }
                            
                            break;

                            case EPhysicalSurface::SurfaceType5:

                            if(Custom_Movement_Component->Get_Gravel_Foot_Step_Sound())
                            {
                                //Only play the foot step sound here if this call is not from the server. If the server is the one making this call its foot steps will be played locally within 
                                //&UCustom_Movement_Component::Server_Play_Foot_Steps_Implementation.
                                if(!Technical_Animator_Character->HasAuthority())
                                {
                                    UGameplayStatics::PlaySoundAtLocation(
                                        Custom_Movement_Component,
                                        Custom_Movement_Component->Get_Gravel_Foot_Step_Sound(),
                                        Right_Foot_Out_Hit.ImpactPoint
                                    );
                                }

                                Custom_Movement_Component->Server_Play_Foot_Steps(Custom_Movement_Component->Get_Gravel_Foot_Step_Sound(), Right_Foot_Out_Hit.ImpactPoint);  
                            }
                            
                            break;

                            case EPhysicalSurface::SurfaceType6:

                            if(Custom_Movement_Component->Get_Heel_Foot_Step_Sound())
                            {
                                //Only play the foot step sound here if this call is not from the server. If the server is the one making this call its foot steps will be played locally within 
                                //&UCustom_Movement_Component::Server_Play_Foot_Steps_Implementation.
                                if(!Technical_Animator_Character->HasAuthority())
                                {
                                    UGameplayStatics::PlaySoundAtLocation(
                                        Custom_Movement_Component,
                                        Custom_Movement_Component->Get_Heel_Foot_Step_Sound(),
                                        Right_Foot_Out_Hit.ImpactPoint
                                    );
                                }

                                Custom_Movement_Component->Server_Play_Foot_Steps(Custom_Movement_Component->Get_Heel_Foot_Step_Sound(), Right_Foot_Out_Hit.ImpactPoint);   
                            }
                            
                            break;

                            case EPhysicalSurface::SurfaceType7:

                            if(Custom_Movement_Component->Get_Leaves_Foot_Step_Sound())
                            {
                                //Only play the foot step sound here if this call is not from the server. If the server is the one making this call its foot steps will be played locally within 
                                //&UCustom_Movement_Component::Server_Play_Foot_Steps_Implementation.
                                if(!Technical_Animator_Character->HasAuthority())
                                {
                                    UGameplayStatics::PlaySoundAtLocation(
                                        Custom_Movement_Component,
                                        Custom_Movement_Component->Get_Leaves_Foot_Step_Sound(),
                                        Right_Foot_Out_Hit.ImpactPoint
                                    );
                                }

                                Custom_Movement_Component->Server_Play_Foot_Steps(Custom_Movement_Component->Get_Leaves_Foot_Step_Sound(), Right_Foot_Out_Hit.ImpactPoint); 
                            }
                            
                            break;

                            case EPhysicalSurface::SurfaceType8:

                            if(Custom_Movement_Component->Get_Low_Stone_Foot_Step_Sound())
                            {
                                //Only play the foot step sound here if this call is not from the server. If the server is the one making this call its foot steps will be played locally within 
                                //&UCustom_Movement_Component::Server_Play_Foot_Steps_Implementation.
                                if(!Technical_Animator_Character->HasAuthority())
                                {
                                    UGameplayStatics::PlaySoundAtLocation(
                                        Custom_Movement_Component,
                                        Custom_Movement_Component->Get_Low_Stone_Foot_Step_Sound(),
                                        Right_Foot_Out_Hit.ImpactPoint
                                    );
                                }
                 
                                Custom_Movement_Component->Server_Play_Foot_Steps(Custom_Movement_Component->Get_Low_Stone_Foot_Step_Sound(), Right_Foot_Out_Hit.ImpactPoint);
                            }
                            
                            break;

                            case EPhysicalSurface::SurfaceType9:

                            if(Custom_Movement_Component->Get_Water_Foot_Step_Sound())
                            {
                                //Only play the foot step sound here if this call is not from the server. If the server is the one making this call its foot steps will be played locally within 
                                //&UCustom_Movement_Component::Server_Play_Foot_Steps_Implementation.
                                if(!Technical_Animator_Character->HasAuthority())
                                {
                                   UGameplayStatics::PlaySoundAtLocation(
                                        Custom_Movement_Component,
                                        Custom_Movement_Component->Get_Water_Foot_Step_Sound(),
                                        Right_Foot_Out_Hit.ImpactPoint
                                    ); 
                                }

                                Custom_Movement_Component->Server_Play_Foot_Steps(Custom_Movement_Component->Get_Water_Foot_Step_Sound(), Right_Foot_Out_Hit.ImpactPoint);
                            }
                            
                            break;

                            default:
                            Debug::Print("Error Selecting Foot Step Sound", FColor::Red, 1000);
                        }
                    }
                }
            }
        }
    }
}

FVector UFoot_Steps::Move_Vector_Up(UCustom_Movement_Component* Custom_Movement_Component, const FVector& Initial_Location, const float& Move_Value)
{
    if(Custom_Movement_Component)
    {
        const FVector Up_Vector{Custom_Movement_Component->UpdatedComponent->GetUpVector()};

        const FVector New_Location{Initial_Location + (Up_Vector * Move_Value)};

        return New_Location;
    }

    else
    {
        return FVector::ZeroVector;
    }
}

FVector UFoot_Steps::Move_Vector_Down(UCustom_Movement_Component* Custom_Movement_Component, const FVector& Initial_Location, const float& Move_Value)
{
    if(Custom_Movement_Component)
    {
        const FVector Down_Vector{-Custom_Movement_Component->UpdatedComponent->GetUpVector()};

        const FVector New_Location{Initial_Location + (Down_Vector * Move_Value)};

        return New_Location;
    }

    else
    {
        return FVector::ZeroVector;
    }
}
