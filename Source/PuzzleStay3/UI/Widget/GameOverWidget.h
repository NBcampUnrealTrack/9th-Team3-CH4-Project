#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"

UCLASS()
class PUZZLESTAY3_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|GameOver")
	void ShowGameOver();

	UFUNCTION(BlueprintCallable, Category = "UI|GameOver")
	void HideGameOver();
};
