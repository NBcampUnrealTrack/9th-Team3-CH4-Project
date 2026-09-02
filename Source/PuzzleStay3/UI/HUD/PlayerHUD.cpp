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

	StartTemporaryUITest();
}

void APlayerHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopTemporaryUITest();

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

// Temporary UI Test Zone. Remove before opening the PR.
void APlayerHUD::StartTemporaryUITest()
{
	ResetTemporaryUITestState();

	if (ViewModel)
	{
		ViewModel->OnDoorActivationRequested.AddUniqueDynamic(this, &APlayerHUD::HandleTemporaryDoorActivationTest);
		ViewModel->OnStage5RoleSelectionRequested.AddUniqueDynamic(this, &APlayerHUD::HandleTemporaryStage5RoleSelectionRequested);
	}

	GetWorldTimerManager().SetTimer(
		TemporaryUITestTimerHandle,
		this,
		&APlayerHUD::UpdateTemporaryUITest,
		1.0f,
		true
	);
}

void APlayerHUD::StopTemporaryUITest()
{
	GetWorldTimerManager().ClearTimer(TemporaryUITestTimerHandle);

	if (ViewModel)
	{
		ViewModel->OnDoorActivationRequested.RemoveDynamic(this, &APlayerHUD::HandleTemporaryDoorActivationTest);
		ViewModel->OnStage5RoleSelectionRequested.RemoveDynamic(this, &APlayerHUD::HandleTemporaryStage5RoleSelectionRequested);
	}
}

void APlayerHUD::ResetTemporaryUITestState()
{
	TemporaryUITestElapsedSeconds = 0;
	LifeCountTestCurrentLife = 3;
	InteractionNotifyTestStep = 0;
	TimerNotifyTestStep = 0;
	bVoiceChatIconTestSpeaking = false;
}

void APlayerHUD::UpdateTemporaryUITest()
{
	if (!ViewModel)
	{
		GetWorldTimerManager().ClearTimer(TemporaryUITestTimerHandle);
		return;
	}

	++TemporaryUITestElapsedSeconds;

	if (TemporaryUITestElapsedSeconds % 3 == 0)
	{
		bVoiceChatIconTestSpeaking = !bVoiceChatIconTestSpeaking;
		ViewModel->RequestVoiceChatSpeaking(bVoiceChatIconTestSpeaking);
	}

	if (TemporaryUITestElapsedSeconds % 5 == 0)
	{
		LifeCountTestCurrentLife = LifeCountTestCurrentLife == 3 ? 2 : 3;
		ViewModel->RequestUpdateLifeCount(LifeCountTestCurrentLife, 5);

		float TimerNotifyTestDuration = 10.0f;
		switch (TimerNotifyTestStep)
		{
		case 0:
			TimerNotifyTestDuration = 10.0f;
			break;
		case 1:
			TimerNotifyTestDuration = 6.0f;
			break;
		case 2:
			TimerNotifyTestDuration = 3.0f;
			break;
		default:
			break;
		}

		ViewModel->RequestTimerNotify(TimerNotifyTestDuration);
		TimerNotifyTestStep = (TimerNotifyTestStep + 1) % 3;
	}

	if (TemporaryUITestElapsedSeconds % 5 == 0)
	{
		switch (InteractionNotifyTestStep)
		{
		case 0:
			ViewModel->RequestShowInteractionNotify(TEXT("TestF"), FText::FromString(TEXT("F")));
			break;
		case 1:
			ViewModel->RequestHideInteractionNotify(TEXT("TestF"));
			break;
		case 2:
			ViewModel->RequestShowInteractionNotify(TEXT("TestG"), FText::FromString(TEXT("G")));
			break;
		case 3:
			ViewModel->RequestHideInteractionNotify(TEXT("TestG"));
			break;
		default:
			break;
		}

		InteractionNotifyTestStep = (InteractionNotifyTestStep + 1) % 4;
	}

	if (TemporaryUITestElapsedSeconds == 7)
	{
		ViewModel->RequestTextNotify(FText::FromString(TEXT("TextNotify Test")), 32.0f, 3.0f);
	}
	else if (TemporaryUITestElapsedSeconds == 10)
	{
		ViewModel->RequestShowTutorialNotify();
	}
	else if (TemporaryUITestElapsedSeconds == 15)
	{
		ViewModel->RequestShowStage5RoleSelect();
	}
	else if (TemporaryUITestElapsedSeconds == 18)
	{
		ViewModel->RequestHideStage5RoleSelect();
	}
	else if (TemporaryUITestElapsedSeconds == 20)
	{
		ViewModel->RequestHideTutorialNotify();
	}
}

void APlayerHUD::HandleTemporaryDoorActivationTest(int32 DoorIndex, bool bIsActive)
{
	UE_LOG(
		LogTemp,
		Log,
		TEXT("Door %d %s"),
		DoorIndex,
		bIsActive ? TEXT("Pressed") : TEXT("Released")
	);
}

void APlayerHUD::HandleTemporaryStage5RoleSelectionRequested(EPS3PlayerRole SelectedRole)
{
	const TCHAR* RoleName = TEXT("Unknown");
	if (SelectedRole == EPS3PlayerRole::Field)
	{
		RoleName = TEXT("Field");
	}
	else if (SelectedRole == EPS3PlayerRole::Screen)
	{
		RoleName = TEXT("Screen");
	}

	UE_LOG(LogTemp, Log, TEXT("Stage5 Role Requested: %s"), RoleName);
}
