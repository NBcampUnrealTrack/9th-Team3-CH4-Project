#include "TextNotifyWidget.h"

#include "Engine/World.h"
#include "../ViewModel/PS3ViewModel.h"

void UTextNotifyWidget::SetViewModel(UPS3ViewModel* InViewModel)
{
	ViewModel = InViewModel;
}

void UTextNotifyWidget::ShowTextNotify(const FText& InDisplayText, float InFontSize, float InDisplayDuration)
{
	if (!ViewModel)
	{
		return;
	}

	ViewModel->SetDisplayText(InDisplayText);
	ViewModel->SetFontSize(InFontSize);
	ViewModel->SetIsTextNotifyVisible(true);

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	World->GetTimerManager().ClearTimer(TextNotifyTimerHandle);

	if (InDisplayDuration > 0.0f)
	{
		World->GetTimerManager().SetTimer(TextNotifyTimerHandle, this, &UTextNotifyWidget::HideTextNotify, InDisplayDuration, false);
	}
	else
	{
		HideTextNotify();
	}
}

void UTextNotifyWidget::HideTextNotify()
{
	if (ViewModel)
	{
		ViewModel->SetIsTextNotifyVisible(false);
	}
}
