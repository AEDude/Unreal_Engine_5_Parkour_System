// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
//#include "Gameplay_Tags/Gameplay_Tags.h"
#include "Native_Gameplay_Tags/Native_Gameplay_Tags.h"
#include "Parkour_Action_Data.generated.h"


class UAnimMontage;

/**
 * 
 */

/*//Struct which declares the data types which will be used in the data asset that is developed by using this class (UParkour_Action_Data) as said Data Asset's parent class.
USTRUCT()
struct FParkour_Action_Settings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FString Element_Name{};

	//Will store the parkour montage to use.
	UPROPERTY(EditAnywhere)
	UAnimMontage* Parkour_Montage;

	//Will store the gameplay tag which is needed to activate the parkour complete.
	UPROPERTY(EditAnywhere)
	FGameplayTag In_State{}; // Element 2 in "TArray<FParkour_Action_Settings> Parkour_Settings" 

	//Will store the gameplay tag that will be set when the parkour montage is complete.
	UPROPERTY(EditAnywhere)
	FGameplayTag Out_State{};

	//The following float variables will store the offset which needs to be applied to the character's root bone when the parkour montage is complete.
	//These offsets are needed because the motion warping which the parkour montages will use won't place the character directly whhere he/she/it needs to be.
	UPROPERTY(EditAnywhere)
	float Warp_1_X_Offset{};

	UPROPERTY(EditAnywhere)
	float Warp_1_Z_Offset{};

	UPROPERTY(EditAnywhere)
	float Warp_2_X_Offset{};

	UPROPERTY(EditAnywhere)
	float Warp_2_Z_Offset{};

	UPROPERTY(EditAnywhere)
	float Warp_3_X_Offset{};

	UPROPERTY(EditAnywhere)
	float Warp_3_Z_Offset{};
}; */


UCLASS()
class TECHNICAL_ANIMATOR_API UParkour_Action_Data : public UPrimaryDataAsset
{
	GENERATED_BODY()

private:

	/*//Constructor
	UParkour_Action_Data();

	 UParkour_Action_Data* Pointer_To_This_Class; */

	/*//An array that is the type of the struct developed above "FParkour_Action_Settings". This array holds all of the data types which are declared in said struct.
	//When a data asset is developed within the editor (using this class (UParkour_Action_Data) as its parent class) and a new array element is created within said data asset, 
	//the fields from all the data types declared in the struct above will be empty, hence ready to be filled each time a new array element is generated or when another data asset which 
	//uses this class (UParkour_Action_Data) as its parent class is created.
	UPROPERTY(EditAnywhere, Category = "Parkour", meta = (TitleProperty = Element_Name))
	TArray<FParkour_Action_Settings> Parkour_Settings{}; 
	
	UPROPERTY(EditAnywhere)
	FString Element_Name{};*/

/*****************************************************************************************************************************************************************************/
	
	
	//Will store the parkour montage to use.
	UPROPERTY(EditAnywhere)
	UAnimMontage* Parkour_Montage;



	//Will store the gameplay tag which is needed to activate the parkour complete.
	UPROPERTY(EditAnywhere)
	FGameplayTag In_State{}; // Element 2 in "TArray<FParkour_Action_Settings> Parkour_Settings" 

	//Will store the gameplay tag that will be set when the parkour montage is complete.
	UPROPERTY(EditAnywhere)
	FGameplayTag Out_State{};



	//Will store the Warp target names used in the animation montages.
	UPROPERTY(EditAnywhere)
	FString Warp_Target_Name_1{};

	UPROPERTY(EditAnywhere)
	FString Warp_Target_Name_2{};

	UPROPERTY(EditAnywhere)
	FString Warp_Target_Name_3{};



	//The following float variables will store the offset which needs to be applied to the character's root bone when the parkour montage is complete.
	//These offsets are needed because the motion warping which the parkour montages will use won't place the character directly whhere he/she/it needs to be.
	UPROPERTY(EditAnywhere)
	float Warp_1_X_Offset{};

	UPROPERTY(EditAnywhere)
	float Warp_1_Y_Offset{};

	UPROPERTY(EditAnywhere)
	float Warp_1_Z_Offset{};

	UPROPERTY(EditAnywhere)
	float Warp_2_X_Offset{};

	UPROPERTY(EditAnywhere)
	float Warp_2_Y_Offset{};

	UPROPERTY(EditAnywhere)
	float Warp_2_Z_Offset{};

	UPROPERTY(EditAnywhere)
	float Warp_3_X_Offset{};

	UPROPERTY(EditAnywhere)
	float Warp_3_Y_Offset{};

	UPROPERTY(EditAnywhere)
	float Warp_3_Z_Offset{};

public:

	//This function will be called from within "&UCustom_Movement_Component::Get_Parkour_Data_Asset()". The input argument which will be passed into this function
	//is a UPROPERTY pointer of the type of this class (UParkour_Action_Data). Said pointer is declared within "UCustom_Movement_Component" and holds the address 
	//to the data asset that is to be used by this function to obtain the information stored within the instance of the array "Parkour_Settings" that said data asset is using
	//The data asset is assigned to said address slot (UParkour_Action_Data* Data_Asset_To_Use) from the character Blueprint within "UCustom_Movement_Component" component.
	//void Get_Parkour_Data_Asset_Information(UParkour_Action_Data* Data_Asset_To_Use);
	
	//UParkour_Action_Data* Get_Pointer_To_This_Class();


	/*The following functions are getter functions which safely return the Animation Montage, FGameplayTag "In_State" and "Out_State", "Warp_Target_Name_1", "Warp_Target_Name_2" and 'Warp_Target_Name_3
	as well as "Warp_1_X_Offset", "Warp_1_Z_Offset", "Warp_2_X_Offset", "Warp_2_Z_Offset", and "Warp_3_X_Offset", "Warp_3_Z_Offset" which are set within the Data_Asset object that is created within the Editor which
	derives from this class. Said object is then stored inside the character Blueprint within the Custom_Movement_Component in its respective 
	"UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement Parkour", meta = (AllowPrivateAccess = "true")) UParkour_Action_Data* Insert_Pointer_Name_Here" */

	FORCEINLINE UAnimMontage* Get_Montage_To_Play() const {return Parkour_Montage;}



	FORCEINLINE FGameplayTag Get_Parkour_In_State() const {return In_State;}

	FORCEINLINE FGameplayTag Get_Parkour_Out_State() const {return Out_State;}



	FORCEINLINE FString Get_Parkour_Warp_Target_Name_1() const {return Warp_Target_Name_1;}

	FORCEINLINE FString Get_Parkour_Warp_Target_Name_2() const {return Warp_Target_Name_2;}

	FORCEINLINE FString Get_Parkour_Warp_Target_Name_3() const {return Warp_Target_Name_3;}



	FORCEINLINE float Get_Parkour_Warp_1_X_Offset() const {return Warp_1_X_Offset;}

	FORCEINLINE float Get_Parkour_Warp_1_Y_Offset() const {return Warp_1_Y_Offset;}

	FORCEINLINE float Get_Parkour_Warp_1_Z_Offset() const {return Warp_1_Z_Offset;}

	FORCEINLINE float Get_Parkour_Warp_2_X_Offset() const {return Warp_2_X_Offset;}

	FORCEINLINE float Get_Parkour_Warp_2_Y_Offset() const {return Warp_2_Y_Offset;}

	FORCEINLINE float Get_Parkour_Warp_2_Z_Offset() const {return Warp_2_Z_Offset;}

	FORCEINLINE float Get_Parkour_Warp_3_X_Offset() const {return Warp_3_X_Offset;}

	FORCEINLINE float Get_Parkour_Warp_3_Y_Offset() const {return Warp_3_Y_Offset;}

	FORCEINLINE float Get_Parkour_Warp_3_Z_Offset() const {return Warp_3_Z_Offset;}
};
