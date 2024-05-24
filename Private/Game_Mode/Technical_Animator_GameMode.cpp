// Copyright Epic Games, Inc. All Rights Reserved.

#include "Game_Mode/Technical_Animator_GameMode.h"
#include "Character/Technical_Animator_Character.h"
#include "UObject/ConstructorHelpers.h"

ATechnical_Animator_GameMode::ATechnical_Animator_GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Core/Character/BP_Technical_Animator_Character"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
