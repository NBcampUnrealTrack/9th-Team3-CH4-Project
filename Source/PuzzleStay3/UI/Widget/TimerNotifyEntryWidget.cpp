#include "TimerNotifyEntryWidget.h"

#include "Components/Image.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace
{
	constexpr float TimerNotifyEntryUpdateInterval = 1.0f / 60.0f;
}

void UTimerNotifyEntryWidget::StartTimer(float InDuration)
{
	ClearTimerUpdateTimer();

	TimerDuration = InDuration;
	TimerStartTime = 0.0f;

	if (TimerDuration <= 0.0f)
	{
		FinishTimer();
		return;
	}

	if (!TimerProgressImage)
	{
		FinishTimer();
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		FinishTimer();
		return;
	}

	TimerStartTime = World->GetTimeSeconds();
	TimerProgressImage->SetVisibility(ESlateVisibility::Visible);
	SetTimerProgress(1.0f);

	World->GetTimerManager().SetTimer(
		TimerUpdateTimerHandle,
		this,
		&UTimerNotifyEntryWidget::UpdateTimerProgress,
		TimerNotifyEntryUpdateInterval,
		true
	);
}

void UTimerNotifyEntryWidget::StopTimer()
{
	ClearTimerUpdateTimer();
	TimerDuration = 0.0f;
	TimerStartTime = 0.0f;

	if (TimerProgressImage)
	{
		TimerProgressImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UTimerNotifyEntryWidget::NativeDestruct()
{
	ClearTimerUpdateTimer();
	Super::NativeDestruct();
}

void UTimerNotifyEntryWidget::UpdateTimerProgress()
{
	UWorld* World = GetWorld();
	if (!World || TimerDuration <= 0.0f)
	{
		FinishTimer();
		return;
	}

	const float ElapsedTime = World->GetTimeSeconds() - TimerStartTime;
	const float Progress = FMath::Clamp(1.0f - (ElapsedTime / TimerDuration), 0.0f, 1.0f);
	SetTimerProgress(Progress);

	if (ElapsedTime >= TimerDuration)
	{
		FinishTimer();
	}
}

void UTimerNotifyEntryWidget::FinishTimer()
{
	SetTimerProgress(0.0f);
	ClearTimerUpdateTimer();
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

void UTimerNotifyEntryWidget::ClearTimerUpdateTimer()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TimerUpdateTimerHandle);
	}
}
