// Fill out your copyright notice in the Description page of Project Settings.


#include "Data_Asset/Parkour_Action_Data.h"
#include "Components/Custom_Movement_Component.h"

/*UParkour_Action_Data::UParkour_Action_Data()
{
    Pointer_To_This_Class = Cast<UParkour_Action_Data>(this);
}*/

/*UParkour_Action_Data* UParkour_Action_Data::Get_Pointer_To_This_Class()
{
    if(Pointer_To_This_Class)
    {
      return Pointer_To_This_Class; 
    }
    
    else
    return nullptr;
}*/

/*void UParkour_Action_Data::Get_Parkour_Data_Asset_Information(UParkour_Action_Data* Data_Asset_To_Use)
{
    //This function will be called from within "&UCustom_Movement_Component::Get_Parkour_Data_Asset()". The input argument which will be passed into this function
	//is a UPROPERTY pointer of the type of this class (UParkour_Action_Data). Said pointer is declared within "UCustom_Movement_Component" and holds the address 
	//to the data asset that is to be used by this function to obtain the information stored within the instance of the array "Parkour_Settings" that said data asset is using via ".GetData()".
	//The data asset is assigned to the address slot of the input argument (UParkour_Action_Data* Data_Asset_To_Use) from the character Blueprint within "UCustom_Movement_Component" component.

    if(Data_Asset_To_Use == nullptr) 
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to use Parkour Data Asset"));
        return;  
    }
    
    else
    {
        Data_Asset_To_Use->Parkour_Settings.GetData();
        UE_LOG(LogTemp, Warning, TEXT("Succeeded in using Parkour Data Asset"));
    }
}*/
