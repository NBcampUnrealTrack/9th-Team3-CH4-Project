#pragma once

#include "CoreMinimal.h"

#include "CosmeticActivationType.generated.h"

UENUM(BlueprintType)
enum class ECosmeticActivationType : uint8
{
	Toggle UMETA(DisplayName = "Toggle"),
	Timed UMETA(DisplayName = "Timed")
};
