#include "TutorialNotifyWidget.h"

namespace
{
	constexpr float TutorialNotifyDuration = 5.0f;
}

void UTutorialNotifyWidget::ShowTutorialNotify()
{
	SetVisibility(ESlateVisibility::Visible);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TutorialNotifyTimerHandle);
		World->GetTimerManager().SetTimer(
			TutorialNotifyTimerHandle,
			this,
			&UTutorialNotifyWidget::HideTutorialNotify,
			TutorialNotifyDuration,
			false);
	}
}

void UTutorialNotifyWidget::HideTutorialNotify()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TutorialNotifyTimerHandle);
	}

	SetVisibility(ESlateVisibility::Collapsed);
}
