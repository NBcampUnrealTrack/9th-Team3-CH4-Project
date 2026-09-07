#pragma once

#include "CoreMinimal.h"
#include "PS3PlayerIdentity.generated.h"

UENUM(BlueprintType)
enum class EPS3PlayerIdentity : uint8
{
	Unassigned UMETA(DisplayName = "Unassigned"),
	Player1 UMETA(DisplayName = "Player 1"),
	Player2 UMETA(DisplayName = "Player 2")
};
