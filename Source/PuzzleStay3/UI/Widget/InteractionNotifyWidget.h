#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionNotifyWidget.generated.h"

UCLASS()
class PUZZLESTAY3_API UInteractionNotifyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|InteractionNotify")
	FText KeyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|InteractionNotify")
	FText ActionName;
};
