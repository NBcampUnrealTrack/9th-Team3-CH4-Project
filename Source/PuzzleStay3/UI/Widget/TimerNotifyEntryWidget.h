#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
	void UpdateTimerState(FName InTimerId, float InMaxTime, float InCurrentTime);

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
	void FinishTimer();
	void SetTimerProgress(float InProgress);

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> TimerProgressMaterial;

	FName TimerId;
	float MaxTime = 0.0f;
	float CurrentTime = 0.0f;
};
