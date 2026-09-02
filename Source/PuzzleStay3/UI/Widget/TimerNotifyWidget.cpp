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

void UTimerNotifyWidget::UpdateTimerNotify(float InDuration)
{
	if (!TimerContainer || !EntryWidgetClass)
	{
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
	TimerEntryWidget->StartTimer(InDuration);
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
