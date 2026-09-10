#include "TimerNotifyWidget.h"

#include "Components/HorizontalBox.h"
#include "TimerNotifyEntryWidget.h"

void UTimerNotifyWidget::ShowTimerNotify()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UTimerNotifyWidget::HideTimerNotifyWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UTimerNotifyWidget::UpdateTimerNotify(FName InTimerId, float InDuration)
{
	if (!TimerContainer || !EntryWidgetClass)
	{
		return;
	}

	if (UTimerNotifyEntryWidget* ExistingTimerEntryWidget = FindActiveTimerEntry(InTimerId))
	{
		ShowTimerNotify();
		ExistingTimerEntryWidget->StartTimer(InTimerId, InDuration);
		return;
	}

	UTimerNotifyEntryWidget* TimerEntryWidget = CreateWidget<UTimerNotifyEntryWidget>(GetOwningPlayer(), EntryWidgetClass);
	if (!TimerEntryWidget)
	{
		return;
	}

	TimerEntryWidget->OnTimerFinished.AddUObject(this, &UTimerNotifyWidget::HandleTimerEntryFinished);
	ActiveTimerEntries.Add(TimerEntryWidget);
	TimerContainer->AddChild(TimerEntryWidget);
	ShowTimerNotify();
	TimerEntryWidget->StartTimer(InTimerId, InDuration);
}

void UTimerNotifyWidget::ReduceTimerNotify(FName InTimerId, float InReduceTime)
{
	if (InTimerId.IsNone() || InReduceTime <= 0.0f)
	{
		return;
	}

	const TArray<TObjectPtr<UTimerNotifyEntryWidget>> TimerEntries = ActiveTimerEntries;
	for (UTimerNotifyEntryWidget* TimerEntryWidget : TimerEntries)
	{
		if (TimerEntryWidget && TimerEntryWidget->GetTimerId() == InTimerId)
		{
			TimerEntryWidget->ReduceRemainingTime(InReduceTime);
			return;
		}
	}
}

void UTimerNotifyWidget::HideTimerNotify()
{
	for (UTimerNotifyEntryWidget* TimerEntryWidget : ActiveTimerEntries)
	{
		if (TimerEntryWidget)
		{
			TimerEntryWidget->OnTimerFinished.RemoveAll(this);
			TimerEntryWidget->StopTimer();
		}
	}

	ActiveTimerEntries.Reset();

	if (TimerContainer)
	{
		TimerContainer->ClearChildren();
	}

	HideTimerNotifyWidget();
}

void UTimerNotifyWidget::NativeDestruct()
{
	HideTimerNotify();
	Super::NativeDestruct();
}

UTimerNotifyEntryWidget* UTimerNotifyWidget::FindActiveTimerEntry(FName InTimerId) const
{
	for (UTimerNotifyEntryWidget* TimerEntryWidget : ActiveTimerEntries)
	{
		if (TimerEntryWidget && TimerEntryWidget->GetTimerId() == InTimerId)
		{
			return TimerEntryWidget;
		}
	}

	return nullptr;
}

void UTimerNotifyWidget::HandleTimerEntryFinished(UTimerNotifyEntryWidget* FinishedEntry)
{
	if (!FinishedEntry)
	{
		return;
	}

	FinishedEntry->OnTimerFinished.RemoveAll(this);
	ActiveTimerEntries.Remove(FinishedEntry);

	if (TimerContainer)
	{
		TimerContainer->RemoveChild(FinishedEntry);
	}

	if (ActiveTimerEntries.Num() == 0)
	{
		HideTimerNotifyWidget();
	}
}
