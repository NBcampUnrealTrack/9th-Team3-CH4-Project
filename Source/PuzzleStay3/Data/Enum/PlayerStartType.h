#pragma once

#include "CoreMinimal.h"
#include "PlayerStartType.generated.h"

UENUM(BlueprintType)
enum class EPlayerStartType : uint8
{
	None UMETA(DisplayName = "None"),
	FieldPlayer UMETA(DisplayName = "FieldPlayer"),
	ScreenPlayer UMETA(DisplayName = "ScreenPlayer"),
	Player1 UMETA(DisplayName = "Player1"),
	Player2 UMETA(DisplayName = "Player2")
};
