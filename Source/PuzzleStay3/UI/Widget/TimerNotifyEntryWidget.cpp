#include "TimerNotifyEntryWidget.h"

#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

void UTimerNotifyEntryWidget::UpdateTimerState(FName InTimerId, float InMaxTime, float InCurrentTime)
{
	TimerId = InTimerId;
	MaxTime = InMaxTime;
	CurrentTime = InCurrentTime;

	if (MaxTime <= 0.0f || CurrentTime <= 0.0f)
	{
		FinishTimer();
		return;
	}

	if (!TimerProgressImage)
	{
		FinishTimer();
		return;
	}

	TimerProgressImage->SetVisibility(ESlateVisibility::Visible);
	SetTimerProgress(CurrentTime / MaxTime);
}

void UTimerNotifyEntryWidget::ReduceRemainingTime(float /*InReduceTime*/)
{
}

FName UTimerNotifyEntryWidget::GetTimerId() const
{
	return TimerId;
}

void UTimerNotifyEntryWidget::StopTimer()
{
	TimerId = NAME_None;
	MaxTime = 0.0f;
	CurrentTime = 0.0f;

	if (TimerProgressImage)
	{
		TimerProgressImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UTimerNotifyEntryWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UTimerNotifyEntryWidget::FinishTimer()
{
	SetTimerProgress(0.0f);
	OnTimerFinished.Broadcast(this);
}

void UTimerNotifyEntryWidget::SetTimerProgress(float InProgress)
{
	if (!TimerProgressImage)
	{
		return;
	}

	if (!TimerProgressMaterial)
	{
		TimerProgressMaterial = TimerProgressImage->GetDynamicMaterial();
	}

	if (TimerProgressMaterial)
	{
		TimerProgressMaterial->SetScalarParameterValue(TEXT("Progress"), FMath::Clamp(InProgress, 0.0f, 1.0f));
	}
}
