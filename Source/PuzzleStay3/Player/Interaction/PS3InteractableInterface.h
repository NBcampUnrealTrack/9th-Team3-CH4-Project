#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PS3InteractableInterface.generated.h"

UINTERFACE(BlueprintType)
class PUZZLESTAY3_API UPS3InteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class PUZZLESTAY3_API IPS3InteractableInterface
{
	GENERATED_BODY()

public:
	// UI 안내 또는 사전 확인용
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PS3|Interaction")
	bool CanInteract(AActor* Requestor) const;

	// 서버에서 호출되는 실제 상호작용
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PS3|Interaction")
	bool Interact(AActor* Requestor);
};