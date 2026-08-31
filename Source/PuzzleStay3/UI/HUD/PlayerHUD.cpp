#include "PlayerHUD.h"

#include "../Widget/DoorOpenButtonWidget.h"
#include "../Widget/InteractionNotifyWidget.h"
#include "../Widget/LifeCountWidget.h"
#include "../Widget/TextNotifyWidget.h"
#include "../Widget/TimerNotifyWidget.h"
#include "../Widget/TutorialNotifyWidget.h"
#include "../ViewModel/PS3ViewModel.h"

APlayerHUD::APlayerHUD()
{
}

void APlayerHUD::SetWidgets(const FPS3HUDWidgets& InWidgets)
{
	Widgets = InWidgets;

	if (Widgets.DoorOpenButtonWidget)
	{
		Widgets.DoorOpenButtonWidget->SetPlayerHUD(this);
	}

	ApplyViewModelToWidgets();
}

void APlayerHUD::SetViewModel(UPS3ViewModel* InViewModel)
{
	ViewModel = InViewModel;

	if (ViewModel)
	{
		ViewModel->SetPlayerHUD(this);
	}

	ApplyViewModelToWidgets();
}

UPS3ViewModel* APlayerHUD::GetViewModel() const
{
	return ViewModel;
}

void APlayerHUD::ShowTextNotify(const FText& InDisplayText, float InFontSize, float InDisplayDuration)
{
	if (!Widgets.TextNotifyWidget)
	{
		return;
	}

	Widgets.TextNotifyWidget->ShowTextNotify(InDisplayText, InFontSize, InDisplayDuration);
}

void APlayerHUD::UpdateLifeCount(int32 InCurrentLifeCount, int32 InMaxLifeCount)
{
	if (!Widgets.LifeCountWidget)
	{
		return;
	}

	Widgets.LifeCountWidget->UpdateLifeCount(InCurrentLifeCount, InMaxLifeCount);
}

void APlayerHUD::ShowInteractionNotify(FName InNotifyId, const FText& InKeyName)
{
	if (!Widgets.InteractionNotifyWidget)
	{
		return;
	}

	Widgets.InteractionNotifyWidget->ShowInteractionNotify(InNotifyId, InKeyName);
}

void APlayerHUD::HideInteractionNotify(FName InNotifyId)
{
	if (!Widgets.InteractionNotifyWidget)
	{
		return;
	}

	Widgets.InteractionNotifyWidget->HideInteractionNotify(InNotifyId);
}

void APlayerHUD::HideAllInteractionNotifies()
{
	if (!Widgets.InteractionNotifyWidget)
	{
		return;
	}

	Widgets.InteractionNotifyWidget->HideAllInteractionNotifies();
}

void APlayerHUD::UpdateTimerNotify(float InRemainingTime, float InTotalTime)
{
	if (!Widgets.TimerNotifyWidget)
	{
		return;
	}

	Widgets.TimerNotifyWidget->UpdateTimerNotify(InRemainingTime, InTotalTime);
}

void APlayerHUD::HideTimerNotify()
{
	if (!Widgets.TimerNotifyWidget)
	{
		return;
	}

	Widgets.TimerNotifyWidget->HideTimerNotify();
}

void APlayerHUD::ShowTutorialNotify()
{
	if (!Widgets.TutorialNotifyWidget)
	{
		return;
	}

	Widgets.TutorialNotifyWidget->ShowTutorialNotify();
}

void APlayerHUD::HideTutorialNotify()
{
	if (!Widgets.TutorialNotifyWidget)
	{
		return;
	}

	Widgets.TutorialNotifyWidget->HideTutorialNotify();
}

void APlayerHUD::UpdateDoorOpenButtons(
	bool bInDoor1Unlocked,
	bool bInDoor2Unlocked,
	bool bInDoor3Unlocked,
	bool bInDoor4Unlocked
)
{
	if (!Widgets.DoorOpenButtonWidget)
	{
		return;
	}

	Widgets.DoorOpenButtonWidget->UpdateDoorOpenButtons(
		bInDoor1Unlocked,
		bInDoor2Unlocked,
		bInDoor3Unlocked,
		bInDoor4Unlocked
	);
}

void APlayerHUD::RequestOpenDoor(int32 InDoorIndex)
{
	if (!ViewModel)
	{
		return;
	}

	ViewModel->RequestOpenDoor(InDoorIndex);
}

void APlayerHUD::ApplyViewModelToWidgets()
{
	if (Widgets.TextNotifyWidget)
	{
		Widgets.TextNotifyWidget->SetViewModel(ViewModel);
	}

	if (Widgets.LifeCountWidget)
	{
		Widgets.LifeCountWidget->SetViewModel(ViewModel);
	}

	if (Widgets.InteractionNotifyWidget)
	{
		Widgets.InteractionNotifyWidget->SetViewModel(ViewModel);
	}

	if (Widgets.TimerNotifyWidget)
	{
		Widgets.TimerNotifyWidget->SetViewModel(ViewModel);
	}

	if (Widgets.DoorOpenButtonWidget)
	{
		Widgets.DoorOpenButtonWidget->SetViewModel(ViewModel);
	}

	if (Widgets.TutorialNotifyWidget)
	{
		Widgets.TutorialNotifyWidget->SetViewModel(ViewModel);
	}
}
