#include "InteractionNotifyWidget.h"

#include "../ViewModel/PS3ViewModel.h"

void UInteractionNotifyWidget::SetViewModel(UPS3ViewModel* InViewModel)
{
	ViewModel = InViewModel;
}

void UInteractionNotifyWidget::ShowInteractionNotify(FName InNotifyId, const FText& InKeyName)
{
	if (InNotifyId.IsNone())
	{
		return;
	}

	ActiveInteractionNotifies.Add(InNotifyId, InKeyName);
	RefreshViewModel();
}

void UInteractionNotifyWidget::HideInteractionNotify(FName InNotifyId)
{
	if (InNotifyId.IsNone())
	{
		return;
	}

	ActiveInteractionNotifies.Remove(InNotifyId);
	RefreshViewModel();
}

void UInteractionNotifyWidget::HideAllInteractionNotifies()
{
	ActiveInteractionNotifies.Empty();
	RefreshViewModel();
}

void UInteractionNotifyWidget::RefreshViewModel()
{
	if (!ViewModel)
	{
		return;
	}

	TArray<FText> InteractionKeyNames;
	InteractionKeyNames.Reserve(ActiveInteractionNotifies.Num());

	for (const TPair<FName, FText>& ActiveInteractionNotify : ActiveInteractionNotifies)
	{
		InteractionKeyNames.Add(ActiveInteractionNotify.Value);
	}

	ViewModel->SetInteractionKeyNames(InteractionKeyNames);
	ViewModel->SetIsInteractionNotifyVisible(!InteractionKeyNames.IsEmpty());
}
