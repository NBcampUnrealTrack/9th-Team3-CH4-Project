#include "DoorOpenButtonWidget.h"

#include "../HUD/PlayerHUD.h"
#include "../ViewModel/PS3ViewModel.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UDoorOpenButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BindDoorButtonDelegates();
	SetDoorPressedFilters(false, false, false, false);
}

void UDoorOpenButtonWidget::NativeDestruct()
{
	UnbindDoorButtonDelegates();
	Super::NativeDestruct();
}

void UDoorOpenButtonWidget::SetViewModel(UPS3ViewModel* InViewModel)
{
	ViewModel = InViewModel;
}

void UDoorOpenButtonWidget::SetPlayerHUD(APlayerHUD* InPlayerHUD)
{
	PlayerHUD = InPlayerHUD;
}

void UDoorOpenButtonWidget::ShowDoorOpenButton()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UDoorOpenButtonWidget::HideDoorOpenButton()
{
	SetVisibility(ESlateVisibility::Collapsed);
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
	SetDoorPressedFilters(!bInDoor1Unlocked, !bInDoor2Unlocked, !bInDoor3Unlocked, !bInDoor4Unlocked);
	ShowDoorOpenButton();
}

void UDoorOpenButtonWidget::SetDoorPressedFilters(
	bool bDoor1Pressed,
	bool bDoor2Pressed,
	bool bDoor3Pressed,
	bool bDoor4Pressed
)
{
	SetPressedFilterVisible(DoorButton1PressedFilter, bDoor1Pressed);
	SetPressedFilterVisible(DoorButton2PressedFilter, bDoor2Pressed);
	SetPressedFilterVisible(DoorButton3PressedFilter, bDoor3Pressed);
	SetPressedFilterVisible(DoorButton4PressedFilter, bDoor4Pressed);
}

void UDoorOpenButtonWidget::RequestDoorActivation(int32 InDoorIndex, bool bIsActive)
{
	if (!PlayerHUD)
	{
		return;
	}

	PlayerHUD->RequestDoorActivation(InDoorIndex, bIsActive);
}

void UDoorOpenButtonWidget::SetPressedFilterVisible(UImage* PressedFilter, bool bVisible)
{
	if (!PressedFilter)
	{
		return;
	}

	PressedFilter->SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
}

void UDoorOpenButtonWidget::BindDoorButtonDelegates()
{
	if (DoorButton1)
	{
		DoorButton1->OnPressed.AddDynamic(this, &UDoorOpenButtonWidget::HandleDoorButton1Pressed);
		DoorButton1->OnReleased.AddDynamic(this, &UDoorOpenButtonWidget::HandleDoorButton1Released);
	}

	if (DoorButton2)
	{
		DoorButton2->OnPressed.AddDynamic(this, &UDoorOpenButtonWidget::HandleDoorButton2Pressed);
		DoorButton2->OnReleased.AddDynamic(this, &UDoorOpenButtonWidget::HandleDoorButton2Released);
	}

	if (DoorButton3)
	{
		DoorButton3->OnPressed.AddDynamic(this, &UDoorOpenButtonWidget::HandleDoorButton3Pressed);
		DoorButton3->OnReleased.AddDynamic(this, &UDoorOpenButtonWidget::HandleDoorButton3Released);
	}

	if (DoorButton4)
	{
		DoorButton4->OnPressed.AddDynamic(this, &UDoorOpenButtonWidget::HandleDoorButton4Pressed);
		DoorButton4->OnReleased.AddDynamic(this, &UDoorOpenButtonWidget::HandleDoorButton4Released);
	}
}

void UDoorOpenButtonWidget::UnbindDoorButtonDelegates()
{
	if (DoorButton1)
	{
		DoorButton1->OnPressed.RemoveDynamic(this, &UDoorOpenButtonWidget::HandleDoorButton1Pressed);
		DoorButton1->OnReleased.RemoveDynamic(this, &UDoorOpenButtonWidget::HandleDoorButton1Released);
	}

	if (DoorButton2)
	{
		DoorButton2->OnPressed.RemoveDynamic(this, &UDoorOpenButtonWidget::HandleDoorButton2Pressed);
		DoorButton2->OnReleased.RemoveDynamic(this, &UDoorOpenButtonWidget::HandleDoorButton2Released);
	}

	if (DoorButton3)
	{
		DoorButton3->OnPressed.RemoveDynamic(this, &UDoorOpenButtonWidget::HandleDoorButton3Pressed);
		DoorButton3->OnReleased.RemoveDynamic(this, &UDoorOpenButtonWidget::HandleDoorButton3Released);
	}

	if (DoorButton4)
	{
		DoorButton4->OnPressed.RemoveDynamic(this, &UDoorOpenButtonWidget::HandleDoorButton4Pressed);
		DoorButton4->OnReleased.RemoveDynamic(this, &UDoorOpenButtonWidget::HandleDoorButton4Released);
	}
}

void UDoorOpenButtonWidget::HandleDoorButton1Pressed()
{
	RequestDoorActivation(1, true);
}

void UDoorOpenButtonWidget::HandleDoorButton1Released()
{
	RequestDoorActivation(1, false);
}

void UDoorOpenButtonWidget::HandleDoorButton2Pressed()
{
	RequestDoorActivation(2, true);
}

void UDoorOpenButtonWidget::HandleDoorButton2Released()
{
	RequestDoorActivation(2, false);
}

void UDoorOpenButtonWidget::HandleDoorButton3Pressed()
{
	RequestDoorActivation(3, true);
}

void UDoorOpenButtonWidget::HandleDoorButton3Released()
{
	RequestDoorActivation(3, false);
}

void UDoorOpenButtonWidget::HandleDoorButton4Pressed()
{
	RequestDoorActivation(4, true);
}

void UDoorOpenButtonWidget::HandleDoorButton4Released()
{
	RequestDoorActivation(4, false);
}
