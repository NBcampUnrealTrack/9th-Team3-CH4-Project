#include "TitleWidget.h"

#include "../HUD/PlayerHUD.h"
#include "Components/Button.h"

void UTitleWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BindTitleButtonDelegates();
}

void UTitleWidget::NativeDestruct()
{
	UnbindTitleButtonDelegates();
	Super::NativeDestruct();
}

void UTitleWidget::SetPlayerHUD(APlayerHUD* InPlayerHUD)
{
	PlayerHUD = InPlayerHUD;
}

void UTitleWidget::ShowTitle()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UTitleWidget::HideTitle()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UTitleWidget::RequestGameStart()
{
	if (!PlayerHUD)
	{
		return;
	}

	PlayerHUD->RequestGameStart();
}

void UTitleWidget::RequestTitleOption()
{
	if (!PlayerHUD)
	{
		return;
	}

	PlayerHUD->RequestTitleOption();
}

void UTitleWidget::RequestGameExit()
{
	if (!PlayerHUD)
	{
		return;
	}

	PlayerHUD->RequestGameExit();
}

void UTitleWidget::BindTitleButtonDelegates()
{
	if (Button_GameStart)
	{
		Button_GameStart->OnClicked.AddUniqueDynamic(this, &UTitleWidget::HandleGameStartButtonClicked);
	}

	if (Button_Setting)
	{
		Button_Setting->OnClicked.AddUniqueDynamic(this, &UTitleWidget::HandleSettingButtonClicked);
	}

	if (Button_GameExit)
	{
		Button_GameExit->OnClicked.AddUniqueDynamic(this, &UTitleWidget::HandleGameExitButtonClicked);
	}
}

void UTitleWidget::UnbindTitleButtonDelegates()
{
	if (Button_GameStart)
	{
		Button_GameStart->OnClicked.RemoveDynamic(this, &UTitleWidget::HandleGameStartButtonClicked);
	}

	if (Button_Setting)
	{
		Button_Setting->OnClicked.RemoveDynamic(this, &UTitleWidget::HandleSettingButtonClicked);
	}

	if (Button_GameExit)
	{
		Button_GameExit->OnClicked.RemoveDynamic(this, &UTitleWidget::HandleGameExitButtonClicked);
	}
}

void UTitleWidget::HandleGameStartButtonClicked()
{
	RequestGameStart();
}

void UTitleWidget::HandleSettingButtonClicked()
{
	RequestTitleOption();
}

void UTitleWidget::HandleGameExitButtonClicked()
{
	RequestGameExit();
}
