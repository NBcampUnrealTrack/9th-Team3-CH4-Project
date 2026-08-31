#pragma once

#include "CoreMinimal.h"

#include "InteractionState.generated.h"

UENUM(BlueprintType)
enum class EInteractionState : uint8
{
	IsNotInteracting UMETA(DisplayName = "Is Not Interacting"),
	IsInteracting UMETA(DisplayName = "Is Interacting")
};

// b기믹1 <-p1 활성화! p1 enum = IsInteracting // b기믹 활성화 로직에서 if ( player enum == IsInteracting) { b기믹과 상호작용 불가능!}  
// else { b기믹과 상호작용 가능}  
//p2 enum = NotIntertacting  
//b기믹2