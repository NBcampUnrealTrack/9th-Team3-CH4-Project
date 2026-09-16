#include "GameOverWidget.h"

#include "../HUD/PlayerHUD.h"
#include "Components/Button.h"
#include "Player/Controller/PS3PlayerControllerBase.h" //준현 수정

void UGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BindGameOverButtonDelegates();
}

void UGameOverWidget::NativeDestruct()
{
	UnbindGameOverButtonDelegates();
	Super::NativeDestruct();
}

void UGameOverWidget::SetPlayerHUD(APlayerHUD* InPlayerHUD)
{
	PlayerHUD = InPlayerHUD;
}

void UGameOverWidget::ShowGameOver()
{
	SetVisibility(ESlateVisibility::Visible);
	if (APS3PlayerControllerBase* PC = Cast<APS3PlayerControllerBase>(GetOwningPlayer()))
	{
		PC->SetGameOverInputMode(true);
		if (PC->IsLocalController() && Button_Restart)
		{
			Button_Restart->SetUserFocus(PC);
		}
	} // 준현 수정
}

void UGameOverWidget::HideGameOver()
{
	SetVisibility(ESlateVisibility::Collapsed);
	if (APS3PlayerControllerBase* PC = Cast<APS3PlayerControllerBase>(GetOwningPlayer()))
	{
		PC->SetGameOverInputMode(false);
	} // 준현 수정
}

void UGameOverWidget::RequestGameRestart()
{
	if (!PlayerHUD)
	{
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("Requesting Game Restart!!!!!!"));
	PlayerHUD->RequestGameRestart();
}

void UGameOverWidget::RequestExitToMain()
{
	if (!PlayerHUD)
	{
		return;
	}

	PlayerHUD->RequestExitToMain();
}

void UGameOverWidget::BindGameOverButtonDelegates()
{
	if (Button_Restart)
	{
		Button_Restart->OnClicked.AddUniqueDynamic(this, &UGameOverWidget::HandleRestartButtonClicked);
	}

	if (Button_ExitToMain)
	{
		Button_ExitToMain->OnClicked.AddUniqueDynamic(this, &UGameOverWidget::HandleExitToMainButtonClicked);
	}
}

void UGameOverWidget::UnbindGameOverButtonDelegates()
{
	if (Button_Restart)
	{
		Button_Restart->OnClicked.RemoveDynamic(this, &UGameOverWidget::HandleRestartButtonClicked);
	}

	if (Button_ExitToMain)
	{
		Button_ExitToMain->OnClicked.RemoveDynamic(this, &UGameOverWidget::HandleExitToMainButtonClicked);
	}
}

void UGameOverWidget::HandleRestartButtonClicked()
{
	RequestGameRestart();
}

void UGameOverWidget::HandleExitToMainButtonClicked()
{
	RequestExitToMain();
}
