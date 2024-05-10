// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Wall_Pipe_Actor.generated.h"

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

	#pragma region Ray_Casts

	void Generate_Custom_Wall_Pipe_Actor_Forward_Vector();

	#pragma endregion
	

	#pragma region Helper_Functions

	FVector Move_Vector_Forward(const FVector& Initial_Location, const FRotator& Rotation, const float& Move_Value);

	#pragma endregion


	#pragma region BP_Variables

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall_Pipe_Forward_Vector", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Wall_Pipe_Forward_Vector_Trace_Types{};

	#pragma endregion

private:
	
	FHitResult Custom_Wall_Pipe_Actor_Forward_Vector_Hit_Result{};

public:	
	
	FORCEINLINE FHitResult Get_Custom_Wall_Pipe_Actor_Forward_Vector_Hit_Result() const {return Custom_Wall_Pipe_Actor_Forward_Vector_Hit_Result;}

};
