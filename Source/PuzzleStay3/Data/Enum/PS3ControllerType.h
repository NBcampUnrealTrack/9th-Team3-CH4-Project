#pragma once

#include "CoreMinimal.h"

#include "PS3ControllerType.generated.h"

UENUM(BlueprintType)
enum class EPS3ControllerType : uint8
{
	ChoiceController UMETA(DisplayName = "ChoiceController"),
	PlayerController UMETA(DisplayName = "PlayerController"),
	ScreenController UMETA(DisplayName = "ScreenController"),
};
