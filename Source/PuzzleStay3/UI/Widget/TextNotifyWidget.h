#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "TextNotifyWidget.generated.h"

class UPS3ViewModel;

UCLASS()
class PUZZLESTAY3_API UTextNotifyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetViewModel(UPS3ViewModel* InViewModel);

	UFUNCTION(BlueprintCallable, Category = "UI|TextNotify")
	void ShowTextNotify(const FText& InDisplayText, float InFontSize, float InDisplayDuration);

	UFUNCTION(BlueprintCallable, Category = "UI|TextNotify")
	void HideTextNotify();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "UI|ViewModel")
	TObjectPtr<UPS3ViewModel> ViewModel;

private:
	FTimerHandle TextNotifyTimerHandle;
};
