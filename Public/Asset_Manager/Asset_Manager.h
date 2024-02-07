// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "Asset_Manager.generated.h"

/**
 * 
 */
UCLASS()
class TECHNICAL_ANIMATOR_API UAsset_Manager : public UAssetManager
{
	GENERATED_BODY()

public:

	//Constructor
	UAsset_Manager();


	//Returns the Asset_Manager singleton object.
	static UAsset_Manager& Get();


protected:

	//Immediately initialize Native Gameplay Tags on engine startup.
	virtual void StartInitialLoading() override;
};
