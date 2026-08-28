#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TextNotifyWidget.generated.h"

UCLASS()
class PUZZLESTAY3_API UTextNotifyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|TextNotify")
	FText DisplayText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|TextNotify")
	float DisplayDuration = 0.0f;
};
