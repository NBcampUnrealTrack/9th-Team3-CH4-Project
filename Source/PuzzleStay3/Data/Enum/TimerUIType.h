#pragma once

#include "CoreMinimal.h"
#include "TimerUIType.generated.h"

UENUM(BlueprintType)
enum class EPS3TimerUIType : uint8
{
	None UMETA(DisplayName = "None"),
	GameStartTimer UMETA(DisplayName = "GameStartTimer"),
	GimmickB_1 UMETA(DisplayName = "GimmickB_1"),
	GimmickB_2 UMETA(DisplayName = "GimmickB_2"),
	GimmickB_3 UMETA(DisplayName = "GimmickB_3"),
	GimmickB_4 UMETA(DisplayName = "GimmickB_4"),
	GimmickB_5 UMETA(DisplayName = "GimmickB_5"),
	GimmickB_6 UMETA(DisplayName = "GimmickB_6"),
	GimmickB_7 UMETA(DisplayName = "GimmickB_7"),
	GimmickB_8 UMETA(DisplayName = "GimmickB_8"),
	GimmickB_9 UMETA(DisplayName = "GimmickB_9"),
	GimmickB_10 UMETA(DisplayName = "GimmickB_10"),
};
