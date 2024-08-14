// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tic_Tac_Actor.generated.h"


class UBoxComponent;
class USceneComponent;
class UWidgetComponent;
class ATechnical_Animator_Character;

UCLASS()
class TECHNICAL_ANIMATOR_API ATic_Tac_Actor : public AActor
{
	GENERATED_BODY()
	

public:	
	// Sets default values for this actor's properties
	ATic_Tac_Actor();


private:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;


	#pragma region Delegate_Callbacks

	UFUNCTION()
	void On_Box_1_Begin_Overlap(UPrimitiveComponent* OverlappedComponent, 
							  AActor* OtherActor, 
							  UPrimitiveComponent* OtherComp, 
							  int32 OtherBodyIndex, 
							  bool bFromSweep, 
							  const FHitResult& SweepResult);

	UFUNCTION()
	void On_Box_1_End_Overlap(UPrimitiveComponent* OverlappedComponent, 
										AActor* OtherActor, 
										UPrimitiveComponent* OtherComp, 
										int32 OtherBodyIndex);

	UFUNCTION()
	void On_Box_2_Begin_Overlap(UPrimitiveComponent* OverlappedComponent, 
							  AActor* OtherActor, 
							  UPrimitiveComponent* OtherComp, 
							  int32 OtherBodyIndex, 
							  bool bFromSweep, 
							  const FHitResult& SweepResult);

	UFUNCTION()
	void On_Box_2_End_Overlap(UPrimitiveComponent* OverlappedComponent, 
										AActor* OtherActor, 
										UPrimitiveComponent* OtherComp, 
										int32 OtherBodyIndex);

	UFUNCTION()
	void On_Box_3_Begin_Overlap(UPrimitiveComponent* OverlappedComponent, 
							  AActor* OtherActor, 
							  UPrimitiveComponent* OtherComp, 
							  int32 OtherBodyIndex, 
							  bool bFromSweep, 
							  const FHitResult& SweepResult);

	UFUNCTION()
	void On_Box_3_End_Overlap(UPrimitiveComponent* OverlappedComponent, 
										AActor* OtherActor, 
										UPrimitiveComponent* OtherComp, 
										int32 OtherBodyIndex);


	#pragma endregion

	#pragma region Pointers

	UPROPERTY(VisibleAnywhere, Category = "Tic_Tac")
	UBoxComponent* Area_Box_1{};

	UPROPERTY(VisibleAnywhere, Category = "Tic_Tac")
	UBoxComponent* Area_Box_2{};

	UPROPERTY(VisibleAnywhere, Category = "Tic_Tac")
	UBoxComponent* Area_Box_3{};

	UPROPERTY(VisibleAnywhere, Category = "Tic_Tac")
	UWidgetComponent* Display_Widget{};

	UPROPERTY()
	USceneComponent* Root_Component{};

	#pragma endregion

	#pragma region Helper_Functions

	FVector Move_Vector_Up(const FVector& Initial_Location, const float& Move_Value) const;

	FVector Move_Vector_Down(const FVector& Initial_Location, const float& Move_Value) const;

	FVector Move_Vector_Backward(const FVector& Initial_Location, const FRotator& Rotation, const float& Move_Value) const;

	#pragma endregion

	void Generate_Tic_Tac_Front_Wall_Hit_Result();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,  Category = "Tic_Tac", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> Generate_Tic_Tac_Front_Wall_Hit_Result_trace_Types{};

	FHitResult Tic_Tac_Front_Wall_Hit_Result{};

	UPROPERTY(EditDefaultsOnly, Category = "Tic_Tac")
	bool bCan_Tic_Tac_Over_Front_Wall{};

protected:

	

public:	

	void Show_Display_Widget(const bool& bShow_Display_Widget);

	FORCEINLINE bool Get_bCan_Tic_Tac_Over_Front_Wall() const {return bCan_Tic_Tac_Over_Front_Wall;}

	FORCEINLINE FHitResult Get_Tic_Tac_Front_Wall_Hit_Result() const {return Tic_Tac_Front_Wall_Hit_Result;}
	

};
