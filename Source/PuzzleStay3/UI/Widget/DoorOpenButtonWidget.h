#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DoorOpenButtonWidget.generated.h"

UCLASS()
class PUZZLESTAY3_API UDoorOpenButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|DoorOpenButton")
	int32 TokenID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|DoorOpenButton")
	bool bIsUnlocked = false;
};
