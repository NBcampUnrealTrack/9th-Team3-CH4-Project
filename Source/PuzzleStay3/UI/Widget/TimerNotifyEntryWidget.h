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
	void StartTimer(FName InTimerId, float InDuration);

	UFUNCTION(BlueprintCallable, Category = "UI|TimerNotify")
	void ReduceRemainingTime(float InReduceTime);

	UFUNCTION(BlueprintPure, Category = "UI|TimerNotify")
	FName GetTimerId() const;

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
	FName TimerId;
	float TotalTime = 0.0f;
	float RemainingTime = 0.0f;
	float LastUpdateTime = 0.0f;
};
