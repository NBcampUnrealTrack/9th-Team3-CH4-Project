#include "PlayerHUD.h"

#include "../Widget/DoorOpenButtonWidget.h"
#include "../Widget/InteractionNotifyWidget.h"
#include "../Widget/LifeCountWidget.h"
#include "../Widget/PS3RootHUDWidget.h"
#include "../Widget/TextNotifyWidget.h"
#include "../Widget/TimerNotifyWidget.h"
#include "../Widget/TutorialNotifyWidget.h"
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

	if (ViewModel)
	{
		UE_LOG(
			LogTemp,
			Log,
			TEXT("RootHUDTest PlayerHUD::BeginPlay BeforeRequestTextNotify ViewModelValid=%s"),
			ViewModel ? TEXT("true") : TEXT("false")
		);
		ViewModel->RequestTextNotify(
			FText::FromString(TEXT("Root HUD TextNotify Test")),
			60.0f,
			3.0f
		);
	}
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
