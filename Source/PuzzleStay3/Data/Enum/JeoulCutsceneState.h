#pragma once

#include "CoreMinimal.h"
#include "JeoulCutsceneState.generated.h"

UENUM(BlueprintType)
enum class EJeoulCutsceneState : uint8
{
	Inactive,
	Playing,
	AwaitingReturn
};
