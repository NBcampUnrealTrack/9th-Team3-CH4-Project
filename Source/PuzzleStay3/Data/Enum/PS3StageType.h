#pragma once

#include "CoreMinimal.h"
#include "PS3StageType.generated.h"

UENUM(BlueprintType)
enum class EPS3StageType : uint8
{
	None UMETA(DisplayName = "None"),
	Stage1 UMETA(DisplayName = "Stage1"),
	Stage2 UMETA(DisplayName = "Stage2"),
	Stage3 UMETA(DisplayName = "Stage3"),
	Stage4 UMETA(DisplayName = "Stage4"),
	Stage5 UMETA(DisplayName = "Stage5"),
	StageEnd UMETA(DisplayName = "StageEnd")
};
