#pragma once

#include "CoreMinimal.h"

#include "CosmeticEffectType.generated.h"

UENUM(BlueprintType)
enum class ECosmeticEffectType : uint8
{
	None UMETA(DisplayName = "None"),
	BlueLight UMETA(DisplayName = "Blue Light"),
	RedLight UMETA(DisplayName = "Red Light"),
	Smoke UMETA(DisplayName = "Smoke"),
	ColorJudgement UMETA(DisplayName = "Color Judgement")
};
