// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Balance_Traversal_Actor.generated.h"

class UBoxComponent;
class USceneComponent;
class UWidgetComponent;

UCLASS()
class TECHNICAL_ANIMATOR_API ABalance_Traversal_Actor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABalance_Traversal_Actor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
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

	#pragma region Traces

	void Generate_Custom_Wall_Pipe_Actor_Forward_Vector();

	#pragma endregion

	#pragma region Helper_Functions

	FVector Move_Vector_Forward(const FVector& Current_Location, const FRotator& Current_Rotation, const float& Move_Value);

	#pragma endregion
	
	#pragma region Variables

	FVector Balance_Traversal_Actor_Forward_Vector{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall_Pipe_Forward_Vector", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Balance_Traversal_Forward_Vector_Trace_Types{};

	#pragma endregion


private:
	
	#pragma region Pointers

	UPROPERTY()
	USceneComponent* Default_Scene_Root{};

	UPROPERTY(VisibleAnywhere, Category = "Balance_Traversal")
	UBoxComponent* Area_Box{};

	UPROPERTY(VisibleAnywhere, Category = "Balance_Traversal")
	UWidgetComponent* Display_Widget{};

	#pragma endregion
	

public:	

	void Show_Display_Widget(const bool& bShow_Display_Widget);
	
	FVector FORCEINLINE Get_Balance_Traversal_Actor_Forward_Vector() const {return Balance_Traversal_Actor_Forward_Vector;}

};
