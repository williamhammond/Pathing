// Copyright Epic Games, Inc. All Rights Reserved.

#include "PathingGameMode.h"
#include "PathingPlayerController.h"
#include "PathingCharacter.h"
#include "UObject/ConstructorHelpers.h"

APathingGameMode::APathingGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = APathingPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}