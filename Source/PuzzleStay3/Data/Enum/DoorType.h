#pragma once

#include "CoreMinimal.h"
#include "DoorType.generated.h"

UENUM(BlueprintType)
enum class EDoorType : uint8
{
	StageAllFinalDoor UMETA(DisplayName = "Stage All Final Door"),
	Stage1NormalDoor UMETA(DisplayName = "Stage 1 Normal Door"),
	Stage4FirstDoor UMETA(DisplayName = "Stage 4 First Door"),
	Stage5NormalDoor UMETA(DisplayName = "Stage 5 Normal Door"),
	Stage5FinalDoor UMETA(DisplayName = "Stage 5 Normal Door")
};
