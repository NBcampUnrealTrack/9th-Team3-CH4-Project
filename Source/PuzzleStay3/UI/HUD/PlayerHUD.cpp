#include "PlayerHUD.h"

#include "../Widget/DoorOpenButtonWidget.h"
#include "../Widget/InteractionNotifyWidget.h"
#include "../Widget/LifeCountWidget.h"
#include "../Widget/PS3RootHUDWidget.h"
#include "../Widget/TextNotifyWidget.h"
#include "../Widget/TimerNotifyWidget.h"
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
		UE_LOG(
			LogTemp,
			Log,
			TEXT("RootHUDTest PlayerHUD::BeginPlay RootHUDWidgetValid=%s"),
			RootHUDWidget ? TEXT("true") : TEXT("false")
		);

		if (RootHUDWidget)
		{
			RootHUDWidget->AddToViewport();

			if (UMVVMView* MVVMView = RootHUDWidget->GetExtension<UMVVMView>())
			{
				const TScriptInterface<INotifyFieldValueChanged> MVVMViewModel = MVVMView->GetViewModel(TEXT("PS3ViewModel"));
				UPS3ViewModel* RootViewModel = Cast<UPS3ViewModel>(MVVMViewModel.GetObject());
				UE_LOG(
					LogTemp,
					Log,
					TEXT("RootHUDTest PlayerHUD::BeginPlay MVVMViewValid=true MVVMSourceValid=%s PS3ViewModelCastValid=%s"),
					MVVMViewModel.GetObject() ? TEXT("true") : TEXT("false"),
					RootViewModel ? TEXT("true") : TEXT("false")
				);
				SetViewModel(RootViewModel);
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("RootHUDTest PlayerHUD::BeginPlay MVVMViewValid=false"));
			}

			FPS3HUDWidgets InitialWidgets = Widgets;
			InitialWidgets.TextNotifyWidget = RootHUDWidget->GetTextNotifyWidget();
			InitialWidgets.LifeCountWidget = RootHUDWidget->GetLifeCountWidget();
			InitialWidgets.InteractionNotifyWidget = RootHUDWidget->GetInteractionNotifyWidget();
			InitialWidgets.TimerNotifyWidget = RootHUDWidget->GetTimerNotifyWidget();
			InitialWidgets.TutorialNotifyWidget = RootHUDWidget->GetTutorialNotifyWidget();
			InitialWidgets.DoorOpenButtonWidget = RootHUDWidget->GetDoorOpenButtonWidget();
			InitialWidgets.VoiceChatIconWidget = RootHUDWidget->GetVoiceChatIconWidget();
			UE_LOG(
				LogTemp,
				Log,
				TEXT("RootHUDTest PlayerHUD::BeginPlay RootTextNotifyWidgetValid=%s RootTextNotifyWidgetClass=%s"),
				InitialWidgets.TextNotifyWidget ? TEXT("true") : TEXT("false"),
				*GetNameSafe(InitialWidgets.TextNotifyWidget ? InitialWidgets.TextNotifyWidget->GetClass() : nullptr)
			);
			SetWidgets(InitialWidgets);
			UE_LOG(
				LogTemp,
				Log,
				TEXT("RootHUDTest PlayerHUD::BeginPlay AfterSetWidgets PlayerHUDTextNotifyWidgetValid=%s"),
				Widgets.TextNotifyWidget ? TEXT("true") : TEXT("false")
			);
		}
	}

	// Temporary UI test code. Remove before opening the PR.
	if (ViewModel)
	{
		ViewModel->OnDoorActivationRequested.AddUniqueDynamic(this, &APlayerHUD::HandleTemporaryDoorActivationTest);

		UE_LOG(
			LogTemp,
			Log,
			TEXT("RootHUDTest PlayerHUD::BeginPlay BeforeRequestTextNotify ViewModelValid=%s"),
			ViewModel ? TEXT("true") : TEXT("false")
		);
		ViewModel->RequestTextNotify(
			FText::FromString(TEXT("이건 5초 뒤 사라지는 테스트 글씨임, 글씨가 길면 아래로 개행되어 내려가야 해서 조금 긴 글씨가 될 수 있으며 무언가를 많이 적어야 해서 지금 생각하고 있음")),
			30.0f,
			5.0f
		);
	}

	if (Widgets.LifeCountWidget)
	{
		TemporaryUITestElapsedSeconds = 0;
		LifeCountTestCurrentLife = 3;
		InteractionNotifyTestStep = 0;
		TimerNotifyTestStep = 0;
		bVoiceChatIconTestSpeaking = false;
		UpdateLifeCount(3, 5);
		if (ViewModel)
		{
			ViewModel->RequestTimerNotify(12.0f);
			ViewModel->RequestHideTutorialNotify();
			ViewModel->RequestVoiceChatSpeaking(false);
		}

		GetWorldTimerManager().SetTimer(
			TemporaryUITestTimerHandle,
			this,
			&APlayerHUD::UpdateTemporaryUITest,
			1.0f,
			true
		);
	}
}

void APlayerHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ViewModel)
	{
		ViewModel->OnDoorActivationRequested.RemoveDynamic(this, &APlayerHUD::HandleTemporaryDoorActivationTest);
	}

	Super::EndPlay(EndPlayReason);
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
	UE_LOG(
		LogTemp,
		Log,
		TEXT("RootHUDTest PlayerHUD::ShowTextNotify Entry Text=%s TextNotifyWidgetValid=%s"),
		*InDisplayText.ToString(),
		Widgets.TextNotifyWidget ? TEXT("true") : TEXT("false")
	);

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

// Temporary UI test code. Remove before opening the PR.
void APlayerHUD::UpdateTemporaryUITest()
{
	if (!Widgets.LifeCountWidget)
	{
		GetWorldTimerManager().ClearTimer(TemporaryUITestTimerHandle);
		return;
	}

	++TemporaryUITestElapsedSeconds;

	if (ViewModel)
	{
		if (TemporaryUITestElapsedSeconds % 5 == 0)
		{
			LifeCountTestCurrentLife = LifeCountTestCurrentLife == 3 ? 2 : 3;
			UpdateLifeCount(LifeCountTestCurrentLife, 5);

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

		if (TemporaryUITestElapsedSeconds == 10)
		{
			ViewModel->RequestShowTutorialNotify();
		}
		else if (TemporaryUITestElapsedSeconds == 20)
		{
			ViewModel->RequestHideTutorialNotify();
		}

		if (TemporaryUITestElapsedSeconds % 3 == 0)
		{
			bVoiceChatIconTestSpeaking = !bVoiceChatIconTestSpeaking;
			ViewModel->RequestVoiceChatSpeaking(bVoiceChatIconTestSpeaking);
		}
	}

	if (Widgets.InteractionNotifyWidget && TemporaryUITestElapsedSeconds % 5 == 0)
	{
		switch (InteractionNotifyTestStep)
		{
		case 0:
			ShowInteractionNotify(TEXT("TestF"), FText::FromString(TEXT("F")));
			break;
		case 1:
			HideInteractionNotify(TEXT("TestF"));
			break;
		case 2:
			ShowInteractionNotify(TEXT("TestG"), FText::FromString(TEXT("G")));
			break;
		case 3:
			HideInteractionNotify(TEXT("TestG"));
			break;
		default:
			break;
		}

		InteractionNotifyTestStep = (InteractionNotifyTestStep + 1) % 4;
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
