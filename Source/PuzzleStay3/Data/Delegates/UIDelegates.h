#pragma once

#include "CoreMinimal.h"

enum class EPS3StageType : uint8;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnScreenPlayerUI, bool)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnFieldPlayerUI, bool)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnStageType, EPS3StageType)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsGameOver, bool)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRoleSelectionUI, bool)
DECLARE_MULTICAST_DELEGATE(FOnStartGameTimer)
DECLARE_MULTICAST_DELEGATE(FOnTimeDeduct)
