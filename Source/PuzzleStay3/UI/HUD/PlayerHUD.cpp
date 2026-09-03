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

void APlayerHUD::HideTextNotify()
{
	if (!Widgets.TextNotifyWidget)
	{
		return;
	}

	Widgets.TextNotifyWidget->HideTextNotify();
}

void APlayerHUD::ShowLifeCount()
{
	if (!Widgets.LifeCountWidget)
	{
		return;
	}

	Widgets.LifeCountWidget->ShowLifeCount();
}

void APlayerHUD::HideLifeCount()
{
	if (!Widgets.LifeCountWidget)
	{
		return;
	}

	Widgets.LifeCountWidget->HideLifeCount();
}

void APlayerHUD::UpdateLifeCount(int32 InCurrentLifeCount, int32 InMaxLifeCount)
{
	if (!Widgets.LifeCountWidget)
	{
		return;
	}

	Widgets.LifeCountWidget->UpdateLifeCount(InCurrentLifeCount, InMaxLifeCount);
}

void APlayerHUD::ShowInteractionNotifyWidget()
{
	if (!Widgets.InteractionNotifyWidget)
	{
		return;
	}

	Widgets.InteractionNotifyWidget->ShowInteractionNotifyWidget();
}

void APlayerHUD::HideInteractionNotifyWidget()
{
	if (!Widgets.InteractionNotifyWidget)
	{
		return;
	}

	Widgets.InteractionNotifyWidget->HideInteractionNotifyWidget();
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

void APlayerHUD::ShowTimerNotify()
{
	if (!Widgets.TimerNotifyWidget)
	{
		return;
	}

	Widgets.TimerNotifyWidget->ShowTimerNotify();
}

void APlayerHUD::HideTimerNotifyWidget()
{
	if (!Widgets.TimerNotifyWidget)
	{
		return;
	}

	Widgets.TimerNotifyWidget->HideTimerNotifyWidget();
}

void APlayerHUD::UpdateTimerNotify(float InDuration)
{
	if (!Widgets.TimerNotifyWidget)
	{
		return;
	}

	Widgets.TimerNotifyWidget->UpdateTimerNotify(InDuration);
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

void APlayerHUD::ShowDoorOpenButton()
{
	if (!Widgets.DoorOpenButtonWidget)
	{
		return;
	}

	Widgets.DoorOpenButtonWidget->ShowDoorOpenButton();
}

void APlayerHUD::HideDoorOpenButton()
{
	if (!Widgets.DoorOpenButtonWidget)
	{
		return;
	}

	Widgets.DoorOpenButtonWidget->HideDoorOpenButton();
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

void APlayerHUD::ShowVoiceChatIcon()
{
	if (!Widgets.VoiceChatIconWidget)
	{
		return;
	}

	Widgets.VoiceChatIconWidget->ShowVoiceChatIcon();
}

void APlayerHUD::HideVoiceChatIcon()
{
	if (!Widgets.VoiceChatIconWidget)
	{
		return;
	}

	Widgets.VoiceChatIconWidget->HideVoiceChatIcon();
}

void APlayerHUD::ShowOptionPopup()
{
	if (!Widgets.OptionPopupWidget)
	{
		return;
	}

	Widgets.OptionPopupWidget->ShowOptionPopup();
}

void APlayerHUD::HideOptionPopup()
{
	if (!Widgets.OptionPopupWidget)
	{
		return;
	}

	Widgets.OptionPopupWidget->HideOptionPopup();
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

void APlayerHUD::ShowTitle()
{
	if (!Widgets.TitleWidget)
	{
		return;
	}

	Widgets.TitleWidget->ShowTitle();
}

void APlayerHUD::HideTitle()
{
	if (!Widgets.TitleWidget)
	{
		return;
	}

	Widgets.TitleWidget->HideTitle();
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

	ViewModel->RequestShowOptionPopup();
}

void APlayerHUD::RequestGameExit()
{
	if (!ViewModel)
	{
		return;
	}

	ViewModel->RequestGameExit();
}

void APlayerHUD::ShowGameOver()
{
	if (!Widgets.GameOverWidget)
	{
		return;
	}

	Widgets.GameOverWidget->ShowGameOver();
}

void APlayerHUD::HideGameOver()
{
	if (!Widgets.GameOverWidget)
	{
		return;
	}

	Widgets.GameOverWidget->HideGameOver();
}

void APlayerHUD::RequestGameRestart()
{
	if (!ViewModel)
	{
		return;
	}

	ViewModel->RequestGameRestart();
}

void APlayerHUD::ShowStage5RoleSelect()
{
	if (!Widgets.Stage5RoleSelectWidget)
	{
		return;
	}

	Widgets.Stage5RoleSelectWidget->ShowStage5RoleSelect();
}

void APlayerHUD::HideStage5RoleSelect()
{
	if (!Widgets.Stage5RoleSelectWidget)
	{
		return;
	}

	Widgets.Stage5RoleSelectWidget->HideStage5RoleSelect();
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
