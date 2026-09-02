#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "TimerNotifyEntryWidget.generated.h"

class UImage;
class UMaterialInstanceDynamic;
class UTimerNotifyEntryWidget;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTimerNotifyEntryFinished, UTimerNotifyEntryWidget*);

UCLASS()
class PUZZLESTAY3_API UTimerNotifyEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnTimerNotifyEntryFinished OnTimerFinished;

	UFUNCTION(BlueprintCallable, Category = "UI|TimerNotify")
	void StartTimer(float InDuration);

	void StopTimer();

protected:
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> TimerProgressImage;

private:
	void UpdateTimerProgress();
	void FinishTimer();
	void SetTimerProgress(float InProgress);
	void ClearTimerUpdateTimer();

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> TimerProgressMaterial;

	FTimerHandle TimerUpdateTimerHandle;
	float TimerDuration = 0.0f;
	float TimerStartTime = 0.0f;
};
