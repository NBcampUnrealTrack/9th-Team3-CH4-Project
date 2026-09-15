#pragma once

#include "CoreMinimal.h"

class UBoxComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnIsInteractionGimmick, const UBoxComponent*, bool)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsGameStart, bool)
DECLARE_MULTICAST_DELEGATE(FOnScreenPlayerSpawned)
DECLARE_MULTICAST_DELEGATE(FOnStageClear)
