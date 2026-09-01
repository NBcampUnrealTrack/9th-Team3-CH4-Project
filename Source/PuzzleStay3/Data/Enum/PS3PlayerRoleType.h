#pragma once

#include "CoreMinimal.h"

#include "PS3PlayerRoleType.generated.h"

UENUM(BlueprintType)
enum class EPS3PlayerRoleType : uint8
{
	PlayerRole_None UMETA(DisplayName = "PlayerRole_None"), 
	PlayerRole_ThirdPerson UMETA(DisplayName = "PlayerRole_ThirdPerson"),
	PlayerRole_Screen UMETA(DisplayName = "PlayerRole_Screen"),
};
