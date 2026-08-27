// Copyright Epic Games, Inc. All Rights Reserved.

#include "PuzzleStay3GameMode.h"
#include "PuzzleStay3Character.h"
#include "UObject/ConstructorHelpers.h"

APuzzleStay3GameMode::APuzzleStay3GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
