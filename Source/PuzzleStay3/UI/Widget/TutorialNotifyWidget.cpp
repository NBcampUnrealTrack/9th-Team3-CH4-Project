#include "TutorialNotifyWidget.h"

#include "../ViewModel/PS3ViewModel.h"

void UTutorialNotifyWidget::SetViewModel(UPS3ViewModel* InViewModel)
{
	ViewModel = InViewModel;
}

void UTutorialNotifyWidget::ShowTutorialNotify()
{
	if (ViewModel)
	{
		ViewModel->SetIsTutorialNotifyVisible(true);
	}
}

void UTutorialNotifyWidget::HideTutorialNotify()
{
	if (ViewModel)
	{
		ViewModel->SetIsTutorialNotifyVisible(false);
	}
}
