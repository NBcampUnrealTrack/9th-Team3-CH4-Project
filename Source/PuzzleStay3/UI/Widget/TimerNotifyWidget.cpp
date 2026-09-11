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

void UTimerNotifyWidget::UpdateTimerNotify(FName InTimerId, float InMaxTime, float InCurrentTime)
{
	if (!TimerContainer || !EntryWidgetClass)
	{
		return;
	}

	if (UTimerNotifyEntryWidget* ExistingTimerEntryWidget = FindActiveTimerEntry(InTimerId))
	{
		ExistingTimerEntryWidget->UpdateTimerState(InTimerId, InMaxTime, InCurrentTime);
		if (InCurrentTime > 0.0f)
		{
			ShowTimerNotify();
		}
		return;
	}

	if (InTimerId.IsNone() || InMaxTime <= 0.0f || InCurrentTime <= 0.0f)
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
	TimerEntryWidget->UpdateTimerState(InTimerId, InMaxTime, InCurrentTime);
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

void UTimerNotifyWidget::ResetTimerNotify()
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
	ResetTimerNotify();
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
