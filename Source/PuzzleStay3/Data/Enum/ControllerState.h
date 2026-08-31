#pragma once

#include "CoreMinimal.h"

#include "ControllerState.generated.h"

UENUM(BlueprintType)
enum class EControllerState : uint8
{
	ChoiceController UMETA(DisplayName = "ChoiceController"),
	PlayerController UMETA(DisplayName = "PlayerController"),
	ScreenController UMETA(DisplayName = "ScreenController"),
};
