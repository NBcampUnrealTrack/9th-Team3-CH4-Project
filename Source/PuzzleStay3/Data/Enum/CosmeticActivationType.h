#pragma once

#include "CoreMinimal.h"

#include "CosmeticActivationType.generated.h"

UENUM(BlueprintType)
enum class ECosmeticActivationType : uint8
{
	SwitchToggle UMETA(DisplayName = "Switch Toggle"),
	SwitchOn UMETA(DisplayName = "Switch On"),
	SwitchTimed UMETA(DisplayName = "Switch Timed"),
	JudgementPersistent UMETA(DisplayName = "Judgement Persistent"),
	JudgementTimed UMETA(DisplayName = "Judgement Timed"),
	DoorProgress UMETA(DisplayName = "Door Progress"),
	TrapTimed UMETA(DisplayName = "Trap Timed")
};
