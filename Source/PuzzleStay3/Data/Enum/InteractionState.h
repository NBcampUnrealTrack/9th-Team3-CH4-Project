#pragma once

#include "CoreMinimal.h"

#include "InteractionState.generated.h"

UENUM(BlueprintType)
enum class EInteractionState : uint8
{
	IsNotInteracting UMETA(DisplayName = "Is Not Interacting"),
	IsInteracting UMETA(DisplayName = "Is Interacting")
};
