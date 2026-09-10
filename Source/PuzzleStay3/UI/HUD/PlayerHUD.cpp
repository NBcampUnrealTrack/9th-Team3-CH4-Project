#include "PlayerHUD.h"

#include "../Widget/DoorOpenButtonWidget.h"
#include "../Widget/GameOverWidget.h"
#include "../Widget/InteractionNotifyWidget.h"
#include "../Widget/LifeCountWidget.h"
#include "../Widget/OptionPopupWidget.h"
#include "../Widget/PS3RootHUDWidget.h"
#include "../Widget/Stage5RoleSelectWidget.h"
#include "../Widget/TextNotifyWidget.h"
#include "../Widget/TimerNotifyWidget.h"
#include "../Widget/TitleWidget.h"
#include "../Widget/TutorialNotifyWidget.h"
#include "../Widget/VoiceChatIconWidget.h"
#include "../ViewModel/PS3ViewModel.h"
#include "View/MVVMView.h"

APlayerHUD::APlayerHUD()
{
}

void APlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	if (RootHUDWidgetClass && !RootHUDWidget)
	{
		RootHUDWidget = CreateWidget<UPS3RootHUDWidget>(GetOwningPlayerController(), RootHUDWidgetClass);

		if (RootHUDWidget)
		{
			RootHUDWidget->AddToViewport();

			if (UMVVMView* MVVMView = RootHUDWidget->GetExtension<UMVVMView>())
			{
				const TScriptInterface<INotifyFieldValueChanged> MVVMViewModel = MVVMView->GetViewModel(TEXT("PS3ViewModel"));
				UPS3ViewModel* RootViewModel = Cast<UPS3ViewModel>(MVVMViewModel.GetObject());
				SetViewModel(RootViewModel);
			}

			FPS3HUDWidgets InitialWidgets = Widgets;
			InitialWidgets.TextNotifyWidget = RootHUDWidget->GetTextNotifyWidget();
			InitialWidgets.LifeCountWidget = RootHUDWidget->GetLifeCountWidget();
			InitialWidgets.InteractionNotifyWidget = RootHUDWidget->GetInteractionNotifyWidget();
			InitialWidgets.TimerNotifyWidget = RootHUDWidget->GetTimerNotifyWidget();
			InitialWidgets.TutorialNotifyWidget = RootHUDWidget->GetTutorialNotifyWidget();
			InitialWidgets.DoorOpenButtonWidget = RootHUDWidget->GetDoorOpenButtonWidget();
			InitialWidgets.VoiceChatIconWidget = RootHUDWidget->GetVoiceChatIconWidget();
			InitialWidgets.OptionPopupWidget = RootHUDWidget->GetOptionPopupWidget();
			InitialWidgets.TitleWidget = RootHUDWidget->GetTitleWidget();
			InitialWidgets.GameOverWidget = RootHUDWidget->GetGameOverWidget();
			InitialWidgets.Stage5RoleSelectWidget = RootHUDWidget->GetStage5RoleSelectWidget();
			SetWidgets(InitialWidgets);
			bIsUIReady = true;
		}
	}
}

void APlayerHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void APlayerHUD::SetWidgets(const FPS3HUDWidgets& InWidgets)
{
	Widgets = InWidgets;

	if (Widgets.DoorOpenButtonWidget)
	{
		Widgets.DoorOpenButtonWidget->SetPlayerHUD(this);
	}

	if (Widgets.Stage5RoleSelectWidget)
	{
		Widgets.Stage5RoleSelectWidget->SetPlayerHUD(this);
	}

	if (Widgets.OptionPopupWidget)
	{
		Widgets.OptionPopupWidget->SetPlayerHUD(this);
	}

	if (Widgets.TitleWidget)
	{
		Widgets.TitleWidget->SetPlayerHUD(this);
	}

	if (Widgets.GameOverWidget)
	{
		Widgets.GameOverWidget->SetPlayerHUD(this);
	}

	ApplyViewModelToWidgets();

	if (ViewModel)
	{
		ViewModel->RefreshStageUI();
	}
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

bool APlayerHUD::IsUIReady() const
{
	return bIsUIReady;
}

void APlayerHUD::ShowTextNotify(EPS3TextNotifyType NotifyType)
{
	if (!Widgets.TextNotifyWidget)
	{
		return;
	}

	Widgets.TextNotifyWidget->ShowTextNotify(NotifyType);
}

void APlayerHUD::SetTextNotifyVisible(bool bVisible)
{
	if (!Widgets.TextNotifyWidget)
	{
		return;
	}

	if (bVisible)
	{
		Widgets.TextNotifyWidget->SetTextNotifyVisible(true);
	}
	else
	{
		Widgets.TextNotifyWidget->SetTextNotifyVisible(false);
	}
}

void APlayerHUD::SetLifeCountVisible(bool bVisible)
{
	if (!Widgets.LifeCountWidget)
	{
		return;
	}

	if (bVisible)
	{
		Widgets.LifeCountWidget->ShowLifeCount();
	}
	else
	{
		Widgets.LifeCountWidget->HideLifeCount();
	}
}

void APlayerHUD::UpdateLifeCount(int32 InCurrentLifeCount, int32 InMaxLifeCount)
{
	if (!Widgets.LifeCountWidget)
	{
		return;
	}

	Widgets.LifeCountWidget->UpdateLifeCount(InCurrentLifeCount, InMaxLifeCount);
}

void APlayerHUD::SetInteractionNotifyVisible(bool bVisible)
{
	if (!Widgets.InteractionNotifyWidget)
	{
		return;
	}

	if (bVisible)
	{
		Widgets.InteractionNotifyWidget->ShowInteractionNotifyWidget();
	}
	else
	{
		Widgets.InteractionNotifyWidget->HideInteractionNotifyWidget();
	}
}

void APlayerHUD::ShowInteractionNotify(EPS3InteractionNotifyType NotifyType)
{
	if (!Widgets.InteractionNotifyWidget)
	{
		return;
	}

	Widgets.InteractionNotifyWidget->ShowInteractionNotify(NotifyType);
}

void APlayerHUD::HideInteractionNotify(EPS3InteractionNotifyType NotifyType)
{
	if (!Widgets.InteractionNotifyWidget)
	{
		return;
	}

	Widgets.InteractionNotifyWidget->HideInteractionNotify(NotifyType);
}

void APlayerHUD::HideAllInteractionNotifies()
{
	if (!Widgets.InteractionNotifyWidget)
	{
		return;
	}

	Widgets.InteractionNotifyWidget->HideAllInteractionNotifies();
}

void APlayerHUD::SetTimerNotifyVisible(bool bVisible)
{
	if (!Widgets.TimerNotifyWidget)
	{
		return;
	}

	if (bVisible)
	{
		Widgets.TimerNotifyWidget->ShowTimerNotify();
	}
	else
	{
		Widgets.TimerNotifyWidget->HideTimerNotifyWidget();
	}
}

void APlayerHUD::UpdateTimerNotify(FName InTimerId, float InMaxTime, float InCurrentTime)
{
	if (!Widgets.TimerNotifyWidget)
	{
		return;
	}

	Widgets.TimerNotifyWidget->UpdateTimerNotify(InTimerId, InMaxTime, InCurrentTime);
}

void APlayerHUD::ReduceTimerNotify(FName InTimerId, float InReduceTime)
{
	if (!Widgets.TimerNotifyWidget)
	{
		return;
	}

	Widgets.TimerNotifyWidget->ReduceTimerNotify(InTimerId, InReduceTime);
}

void APlayerHUD::ResetTimerNotify()
{
	if (!Widgets.TimerNotifyWidget)
	{
		return;
	}

	Widgets.TimerNotifyWidget->ResetTimerNotify();
}

void APlayerHUD::SetTutorialNotifyVisible(bool bVisible)
{
	if (!Widgets.TutorialNotifyWidget)
	{
		return;
	}

	if (bVisible)
	{
		Widgets.TutorialNotifyWidget->ShowTutorialNotify();
	}
	else
	{
		Widgets.TutorialNotifyWidget->HideTutorialNotify();
	}
}

void APlayerHUD::SetDoorOpenButtonVisible(bool bVisible)
{
	if (!Widgets.DoorOpenButtonWidget)
	{
		return;
	}

	if (bVisible)
	{
		Widgets.DoorOpenButtonWidget->ShowDoorOpenButton();
	}
	else
	{
		Widgets.DoorOpenButtonWidget->HideDoorOpenButton();
	}
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

void APlayerHUD::RequestDoorActivation(int32 InDoorIndex, bool bIsActive)
{
	if (!ViewModel)
	{
		return;
	}

	ViewModel->RequestDoorActivation(InDoorIndex, bIsActive);
}

void APlayerHUD::UpdateVoiceChatIcon(bool bInIsSpeaking)
{
	if (!Widgets.VoiceChatIconWidget)
	{
		return;
	}

	Widgets.VoiceChatIconWidget->SetSpeaking(bInIsSpeaking);
}

void APlayerHUD::SetVoiceChatIconVisible(bool bVisible)
{
	if (!Widgets.VoiceChatIconWidget)
	{
		return;
	}

	if (bVisible)
	{
		Widgets.VoiceChatIconWidget->ShowVoiceChatIcon();
	}
	else
	{
		Widgets.VoiceChatIconWidget->HideVoiceChatIcon();
	}
}

void APlayerHUD::SetOptionPopupVisible(bool bVisible)
{
	if (!Widgets.OptionPopupWidget)
	{
		return;
	}

	if (bVisible)
	{
		Widgets.OptionPopupWidget->ShowOptionPopup();
	}
	else
	{
		Widgets.OptionPopupWidget->HideOptionPopup();
	}
}

void APlayerHUD::ToggleOptionPopup()
{
	if (!Widgets.OptionPopupWidget)
	{
		return;
	}

	Widgets.OptionPopupWidget->ToggleOptionPopup();
}

void APlayerHUD::RequestExitToMain()
{
	if (!ViewModel)
	{
		return;
	}

	ViewModel->RequestExitToMain();
}

void APlayerHUD::RequestBGMVolumeChanged(float Value)
{
	if (!ViewModel)
	{
		return;
	}

	ViewModel->RequestBGMVolumeChanged(Value);
}

void APlayerHUD::RequestSFXVolumeChanged(float Value)
{
	if (!ViewModel)
	{
		return;
	}

	ViewModel->RequestSFXVolumeChanged(Value);
}

void APlayerHUD::RequestVoiceChatEnabledChanged(bool bEnabled)
{
	if (!ViewModel)
	{
		return;
	}

	ViewModel->RequestVoiceChatEnabledChanged(bEnabled);
}

void APlayerHUD::RequestResolutionChanged(const FString& Resolution)
{
	if (!ViewModel)
	{
		return;
	}

	ViewModel->RequestResolutionChanged(Resolution);
}

void APlayerHUD::SetTitleVisible(bool bVisible)
{
	if (!Widgets.TitleWidget)
	{
		return;
	}

	if (bVisible)
	{
		Widgets.TitleWidget->ShowTitle();
	}
	else
	{
		Widgets.TitleWidget->HideTitle();
	}
}

void APlayerHUD::RequestGameStart()
{
	if (!ViewModel)
	{
		return;
	}

	ViewModel->RequestGameStart();
}

void APlayerHUD::RequestTitleOption()
{
	if (!ViewModel)
	{
		return;
	}

	ViewModel->RequestSetOptionPopupVisible(true);
}

void APlayerHUD::RequestGameExit()
{
	if (!ViewModel)
	{
		return;
	}

	ViewModel->RequestGameExit();
}

void APlayerHUD::SetGameOverVisible(bool bVisible)
{
	if (!Widgets.GameOverWidget)
	{
		return;
	}

	if (bVisible)
	{
		Widgets.GameOverWidget->ShowGameOver();
	}
	else
	{
		Widgets.GameOverWidget->HideGameOver();
	}
}

void APlayerHUD::RequestGameRestart()
{
	if (!ViewModel)
	{
		return;
	}

	ViewModel->RequestGameRestart();
}

void APlayerHUD::SetStage5RoleSelectVisible(bool bVisible)
{
	if (!Widgets.Stage5RoleSelectWidget)
	{
		return;
	}

	if (bVisible)
	{
		Widgets.Stage5RoleSelectWidget->ShowStage5RoleSelect();
	}
	else
	{
		Widgets.Stage5RoleSelectWidget->HideStage5RoleSelect();
	}
}

void APlayerHUD::RequestStage5RoleSelection(EPS3PlayerRole SelectedRole)
{
	if (!ViewModel)
	{
		return;
	}

	ViewModel->RequestStage5RoleSelection(SelectedRole);
}

void APlayerHUD::ApplyViewModelToWidgets()
{
	if (Widgets.LifeCountWidget)
	{
		Widgets.LifeCountWidget->SetViewModel(ViewModel);
	}

	if (Widgets.DoorOpenButtonWidget)
	{
		Widgets.DoorOpenButtonWidget->SetViewModel(ViewModel);
	}

}
