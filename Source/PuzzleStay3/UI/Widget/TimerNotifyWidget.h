#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TimerNotifyWidget.generated.h"

class UPS3ViewModel;

UCLASS()
class PUZZLESTAY3_API UTimerNotifyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetViewModel(UPS3ViewModel* InViewModel);

	UFUNCTION(BlueprintCallable, Category = "UI|TimerNotify")
	void UpdateTimerNotify(float InRemainingTime, float InTotalTime);

	UFUNCTION(BlueprintCallable, Category = "UI|TimerNotify")
	void HideTimerNotify();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "UI|ViewModel")
	TObjectPtr<UPS3ViewModel> ViewModel;
};
