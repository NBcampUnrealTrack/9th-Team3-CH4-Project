#include "TimerNotifyEntryWidget.h"

#include "Components/Image.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace
{
	constexpr float TimerNotifyEntryUpdateInterval = 1.0f / 60.0f;
}

void UTimerNotifyEntryWidget::StartTimer(FName InTimerId, float InDuration)
{
	ClearTimerUpdateTimer();

	TimerId = InTimerId;
	TotalTime = InDuration;
	RemainingTime = InDuration;
	LastUpdateTime = 0.0f;

	if (TotalTime <= 0.0f)
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

	LastUpdateTime = World->GetTimeSeconds();
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

void UTimerNotifyEntryWidget::ReduceRemainingTime(float InReduceTime)
{
	if (InReduceTime <= 0.0f || TotalTime <= 0.0f)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		const float CurrentTime = World->GetTimeSeconds();
		const float DeltaTime = FMath::Max(CurrentTime - LastUpdateTime, 0.0f);
		LastUpdateTime = CurrentTime;
		RemainingTime = FMath::Max(RemainingTime - DeltaTime, 0.0f);
	}

	RemainingTime = FMath::Max(RemainingTime - InReduceTime, 0.0f);
	SetTimerProgress(RemainingTime / TotalTime);

	if (RemainingTime <= 0.0f)
	{
		FinishTimer();
	}
}

FName UTimerNotifyEntryWidget::GetTimerId() const
{
	return TimerId;
}

void UTimerNotifyEntryWidget::StopTimer()
{
	ClearTimerUpdateTimer();
	TimerId = NAME_None;
	TotalTime = 0.0f;
	RemainingTime = 0.0f;
	LastUpdateTime = 0.0f;

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
	if (!World || TotalTime <= 0.0f)
	{
		FinishTimer();
		return;
	}

	const float CurrentTime = World->GetTimeSeconds();
	const float DeltaTime = FMath::Max(CurrentTime - LastUpdateTime, 0.0f);
	LastUpdateTime = CurrentTime;

	RemainingTime = FMath::Max(RemainingTime - DeltaTime, 0.0f);
	SetTimerProgress(RemainingTime / TotalTime);

	if (RemainingTime <= 0.0f)
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
