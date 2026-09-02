#include "Stage5RoleSelectWidget.h"

#include "../HUD/PlayerHUD.h"
#include "Components/Button.h"

void UStage5RoleSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BindStage5RoleButtonDelegates();
}

void UStage5RoleSelectWidget::NativeDestruct()
{
	UnbindStage5RoleButtonDelegates();
	Super::NativeDestruct();
}

void UStage5RoleSelectWidget::SetPlayerHUD(APlayerHUD* InPlayerHUD)
{
	PlayerHUD = InPlayerHUD;
}

void UStage5RoleSelectWidget::ShowStage5RoleSelect()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UStage5RoleSelectWidget::HideStage5RoleSelect()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UStage5RoleSelectWidget::RequestStage5RoleSelection(EPS3PlayerRole SelectedRole)
{
	if (!PlayerHUD)
	{
		return;
	}

	PlayerHUD->RequestStage5RoleSelection(SelectedRole);
}

void UStage5RoleSelectWidget::BindStage5RoleButtonDelegates()
{
	if (Button_Field)
	{
		Button_Field->OnClicked.AddUniqueDynamic(this, &UStage5RoleSelectWidget::HandleFieldButtonClicked);
	}

	if (Button_Screen)
	{
		Button_Screen->OnClicked.AddUniqueDynamic(this, &UStage5RoleSelectWidget::HandleScreenButtonClicked);
	}
}

void UStage5RoleSelectWidget::UnbindStage5RoleButtonDelegates()
{
	if (Button_Field)
	{
		Button_Field->OnClicked.RemoveDynamic(this, &UStage5RoleSelectWidget::HandleFieldButtonClicked);
	}

	if (Button_Screen)
	{
		Button_Screen->OnClicked.RemoveDynamic(this, &UStage5RoleSelectWidget::HandleScreenButtonClicked);
	}
}

void UStage5RoleSelectWidget::HandleFieldButtonClicked()
{
	RequestStage5RoleSelection(EPS3PlayerRole::Field);
}

void UStage5RoleSelectWidget::HandleScreenButtonClicked()
{
	RequestStage5RoleSelection(EPS3PlayerRole::Screen);
}
