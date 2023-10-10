// Copyright Epic Games, Inc. All Rights Reserved.

#include "Technical_AnimatorGameMode.h"
#include "Technical_AnimatorCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATechnical_AnimatorGameMode::ATechnical_AnimatorGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
