#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TutorialNotifyWidget.generated.h"

UCLASS()
class PUZZLESTAY3_API UTutorialNotifyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|TutorialNotify")
	void ShowTutorialNotify();

	UFUNCTION(BlueprintCallable, Category = "UI|TutorialNotify")
	void HideTutorialNotify();
};
