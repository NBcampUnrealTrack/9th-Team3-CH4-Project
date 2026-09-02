#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleWidget.generated.h"

UCLASS()
class PUZZLESTAY3_API UTitleWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|Title")
	void ShowTitle();

	UFUNCTION(BlueprintCallable, Category = "UI|Title")
	void HideTitle();
};
