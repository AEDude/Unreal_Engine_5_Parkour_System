// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Wall_Pipe_Actor.generated.h"

class USceneComponent;
class UCapsuleComponent;
class UWidgetComponent;

UCLASS()
class TECHNICAL_ANIMATOR_API AWall_Pipe_Actor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWall_Pipe_Actor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	#pragma region Delegate_Callbacks

	UFUNCTION()
	void On_Capsule_Begin_Overlap(UPrimitiveComponent* OverlappedComponent, 
							AActor* OtherActor, 
							UPrimitiveComponent* OtherComp, 
							int32 OtherBodyIndex,
							bool bFromSweep, 
							const FHitResult& SweepResult);

	UFUNCTION()
	void On_Capsule_End_Overlap(UPrimitiveComponent* OverlappedComponent, 
							    AActor* OtherActor, 
								UPrimitiveComponent* OtherComp, 
								int32 OtherBodyIndex);

	#pragma endregion

	#pragma region Ray_Casts

	void Generate_Custom_Wall_Pipe_Actor_Forward_Vector();

	#pragma endregion
	
	#pragma region Helper_Functions

	FVector Move_Vector_Forward(const FVector& Initial_Location, const FRotator& Rotation, const float& Move_Value);

	#pragma endregion

	#pragma region Variables

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall_Pipe_Forward_Vector", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Wall_Pipe_Forward_Vector_Trace_Types{};

	FHitResult Custom_Wall_Pipe_Actor_Forward_Vector_Hit_Result{};

	#pragma endregion

private:

	#pragma region Pointers

	UPROPERTY()
	USceneComponent* Default_Scene_Root{};
	
	UPROPERTY(VisibleAnywhere, Category = "Wall_Pipe")
	UCapsuleComponent* Area_Capsule{};

	UPROPERTY(VisibleAnywhere, Category = "Wall_Pipe")
	UWidgetComponent* Display_Widget{};

	#pragma endregion

public:	
	
	FORCEINLINE FHitResult Get_Custom_Wall_Pipe_Actor_Forward_Vector_Hit_Result() const {return Custom_Wall_Pipe_Actor_Forward_Vector_Hit_Result;}

	void Set_Wall_Pipe_Actor_Widget_Visibility(const bool& bDisplay_Widget);

};
