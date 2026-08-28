#include "LifeCountWidget.h"

#include "../ViewModel/PS3ViewModel.h"

void ULifeCountWidget::SetViewModel(UPS3ViewModel* InViewModel)
{
	ViewModel = InViewModel;
}

void ULifeCountWidget::UpdateLifeCount(int32 InCurrentLifeCount, int32 InMaxLifeCount)
{
	if (!ViewModel)
	{
		return;
	}

	ViewModel->SetCurrentLifeCount(InCurrentLifeCount);
	ViewModel->SetMaxLifeCount(InMaxLifeCount);
}
