#pragma once

#include "CoreMinimal.h"

#include "VoiceChatState.generated.h"

UENUM(BlueprintType)
enum class EVoiceChatState : uint8
{
	Default UMETA(DisplayName = "Default"),
	Conversion UMETA(DisplayName = "Conversion"),
	Inactive UMETA(DisplayName = "Inactive"),
};
