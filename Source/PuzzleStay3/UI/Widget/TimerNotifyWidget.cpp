#include "TimerNotifyWidget.h"

#include "../ViewModel/PS3ViewModel.h"

void UTimerNotifyWidget::SetViewModel(UPS3ViewModel* InViewModel)
{
	ViewModel = InViewModel;
}

void UTimerNotifyWidget::UpdateTimerNotify(float InRemainingTime, float InTotalTime)
{
	if (!ViewModel)
	{
		return;
	}

	float Progress = 0.0f;

	if (InTotalTime > 0.0f)
	{
		Progress = FMath::Clamp(
			InRemainingTime / InTotalTime,
			0.0f,
			1.0f
		);
	}

	ViewModel->SetTimerNotifyProgress(Progress);
	ViewModel->SetIsTimerNotifyVisible(true);
}

void UTimerNotifyWidget::HideTimerNotify()
{
	if (ViewModel)
	{
		ViewModel->SetIsTimerNotifyVisible(false);
	}
}
