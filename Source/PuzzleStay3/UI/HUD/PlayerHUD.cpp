#include "PlayerHUD.h"

#include "../Widget/InteractionNotifyWidget.h"
#include "../Widget/LifeCountWidget.h"
#include "../Widget/TextNotifyWidget.h"
#include "../Widget/TimerNotifyWidget.h"
#include "../Widget/TutorialNotifyWidget.h"

APlayerHUD::APlayerHUD()
{
}

void APlayerHUD::SetWidgets(const FPS3HUDWidgets& InWidgets)
{
	Widgets = InWidgets;
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
