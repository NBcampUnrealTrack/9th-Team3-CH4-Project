#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TimerNotifyWidget.generated.h"

class UHorizontalBox;
class UTimerNotifyEntryWidget;

UCLASS()
class PUZZLESTAY3_API UTimerNotifyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|TimerNotify")
	void ShowTimerNotify();

	UFUNCTION(BlueprintCallable, Category = "UI|TimerNotify")
	void HideTimerNotifyWidget();

	UFUNCTION(BlueprintCallable, Category = "UI|TimerNotify")
	void UpdateTimerNotify(FName InTimerId, float InDuration);

	UFUNCTION(BlueprintCallable, Category = "UI|TimerNotify")
	void ReduceTimerNotify(FName InTimerId, float InReduceTime);

	UFUNCTION(BlueprintCallable, Category = "UI|TimerNotify")
	void ResetTimerNotify();

protected:
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UHorizontalBox> TimerContainer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|TimerNotify")
	TSubclassOf<UTimerNotifyEntryWidget> EntryWidgetClass;

private:
	UTimerNotifyEntryWidget* FindActiveTimerEntry(FName InTimerId) const;
	void HandleTimerEntryFinished(UTimerNotifyEntryWidget* FinishedEntry);

	UPROPERTY()
	TArray<TObjectPtr<UTimerNotifyEntryWidget>> ActiveTimerEntries;
};
