#pragma once

#include "CoreMinimal.h"
#include "PS3TextNotifyType.generated.h"

UENUM(BlueprintType)
enum class EPS3TextNotifyType : uint8
{
	None,
	Stage1,
	Stage2,
	Stage3,
	Stage4,
	Stage5,
	DoorOpenedSound,
	DoorClosedSound
};
