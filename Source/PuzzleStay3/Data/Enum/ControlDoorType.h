#pragma once

#include "CoreMinimal.h"
#include "ControlDoorType.generated.h"

UENUM(BlueprintType)
enum class EControlDoorType : uint8
{
	None UMETA(DisplayName = "None"),
	Door_A UMETA(DisplayName = "Door_A"),
	Door_B UMETA(DisplayName = "Door_B"),
	Door_C UMETA(DisplayName = "Door_C"),
	Door_D UMETA(DisplayName = "Door_D")
};
