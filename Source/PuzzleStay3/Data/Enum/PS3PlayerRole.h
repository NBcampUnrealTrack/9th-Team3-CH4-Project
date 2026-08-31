#pragma once

#include "CoreMinimal.h"
#include "PS3PlayerRole.generated.h"

UENUM(BlueprintType)
enum class EPS3PlayerRole : uint8
{
	Unassigned UMETA(DisplayName = "Unassigned"),
	Field UMETA(DisplayName = "Field Player"),
	Screen UMETA(DisplayName = "Screen Player")
};
