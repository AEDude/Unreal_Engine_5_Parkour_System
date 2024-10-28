// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Wall_Vault_Actor.generated.h"


class USceneComponent;
class UBoxComponent;

UCLASS()
class TECHNICAL_ANIMATOR_API AWall_Vault_Actor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWall_Vault_Actor();


private:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	#pragma region Delegate_Callbacks

	UFUNCTION()
	void On_Box_Begin_Overlap(UPrimitiveComponent* OverlappedComponent, 
									AActor* OtherActor, 
									UPrimitiveComponent* OtherComp, 
									int32 OtherBodyIndex, 
									bool bFromSweep, 
									const FHitResult & SweepResult);
	UFUNCTION()
	void On_Box_End_Overlap(UPrimitiveComponent* OverlappedComponent, 
							AActor* OtherActor, 
							UPrimitiveComponent* OtherComp, 
							int32 OtherBodyIndex);


	#pragma endregion
	
	#pragma region Pointers

	UPROPERTY(VisibleAnywhere, Category = "Wall_Vault")
	USceneComponent* Default_Scene_Root{};
	
	UPROPERTY(VisibleAnywhere, Category = "Wall_Vault")
	UBoxComponent* Area_Box_1{};

	UPROPERTY(VisibleAnywhere, Category = "Wall_Vault")
	UBoxComponent* Area_Box_2{};

	UPROPERTY(VisibleAnywhere, Category = "Wall_Vault")
	UBoxComponent* Area_Box_3{};

	UPROPERTY(VisibleAnywhere, Category = "Wall_Vault")
	UBoxComponent* Area_Box_4{};

	#pragma endregion

	#pragma region Bluprint_Variables	

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parkour_Wall_Vault_Type", meta = (AllowPrivateAccess = "true"))
	bool bEnable_Wall_Under_Bar{false};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parkour_Wall_Vault_Type", meta = (AllowPrivateAccess = "true"))
	bool bEnable_Over_Wall_180_Shimmy{false};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace_Types", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Get_Wall_Top_Location_Trace_Types{};

	#pragma endregion

	#pragma region Helper_Functions

	FVector Move_Vector_Up(const FVector& Initial_Location, const float& Move_Value) const;

	FVector Move_Vector_Down(const FVector& Initial_Location, const float& Move_Value) const;

	#pragma endregion

	#pragma region Ray_Casts

	void Obtain_Warp_Point_Hit_Result();

	#pragma endregion

	#pragma region FHitResults

	FHitResult Wall_Vault_Actor_Warp_Point_Hit_Result{};

	#pragma endregion

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
	FORCEINLINE bool Get_bEnable_Wall_Under_Bar() const {return bEnable_Wall_Under_Bar;}

	FORCEINLINE bool Get_bEnable_Over_Wall_180_Shimmy() const {return bEnable_Over_Wall_180_Shimmy;}

	FORCEINLINE FHitResult Get_Wall_Vault_Actor_Warp_Point_Hit_Result() const {return Wall_Vault_Actor_Warp_Point_Hit_Result;}

};
