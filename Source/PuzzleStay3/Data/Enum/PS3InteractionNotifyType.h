#pragma once

#include "CoreMinimal.h"
#include "PS3InteractionNotifyType.generated.h"

UENUM(BlueprintType)
enum class EPS3InteractionNotifyType : uint8
{
	Interact,
	Drop
};
