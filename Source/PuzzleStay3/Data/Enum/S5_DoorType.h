#pragma once

#include "CoreMinimal.h"
#include "S5_DoorType.generated.h"

UENUM(BlueprintType)
enum class ES5_DoorType : uint8
{
	Door_A UMETA(DisplayName = "Door_A"),
	Door_B UMETA(DisplayName = "Door_B"),
	Door_C UMETA(DisplayName = "Door_C"),
	Door_D UMETA(DisplayName = "Door_D")
};
