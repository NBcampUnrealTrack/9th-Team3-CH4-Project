#include "DoorOpenButtonWidget.h"

#include "../HUD/PlayerHUD.h"
#include "../ViewModel/PS3ViewModel.h"

void UDoorOpenButtonWidget::SetViewModel(UPS3ViewModel* InViewModel)
{
	ViewModel = InViewModel;
}

void UDoorOpenButtonWidget::SetPlayerHUD(APlayerHUD* InPlayerHUD)
{
	PlayerHUD = InPlayerHUD;
}

void UDoorOpenButtonWidget::UpdateDoorOpenButtons(
	bool bInDoor1Unlocked,
	bool bInDoor2Unlocked,
	bool bInDoor3Unlocked,
	bool bInDoor4Unlocked
)
{
	if (!ViewModel)
	{
		return;
	}

	ViewModel->SetIsDoor1Unlocked(bInDoor1Unlocked);
	ViewModel->SetIsDoor2Unlocked(bInDoor2Unlocked);
	ViewModel->SetIsDoor3Unlocked(bInDoor3Unlocked);
	ViewModel->SetIsDoor4Unlocked(bInDoor4Unlocked);
}

void UDoorOpenButtonWidget::RequestOpenDoor(int32 InDoorIndex)
{
	if (!PlayerHUD)
	{
		return;
	}

	PlayerHUD->RequestOpenDoor(InDoorIndex);
}
