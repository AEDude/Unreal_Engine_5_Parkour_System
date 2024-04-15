// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "Player_Camera_Manager.generated.h"

/**
 * 
 */
UCLASS()
class TECHNICAL_ANIMATOR_API APlayer_Camera_Manager : public APlayerCameraManager
{
	GENERATED_BODY()

	protected:

		virtual void UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime) override;

		FVector New_Camera_Location{};

		FRotator New_Camera_Rotation{};

		float New_Camera_FOV{85};
	
};
