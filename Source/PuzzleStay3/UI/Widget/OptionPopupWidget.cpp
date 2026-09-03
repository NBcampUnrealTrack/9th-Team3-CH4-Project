#include "OptionPopupWidget.h"

#include "../HUD/PlayerHUD.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/Slider.h"

void UOptionPopupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeResolutionOptions();
	BindOptionPopupDelegates();
}

void UOptionPopupWidget::NativeDestruct()
{
	UnbindOptionPopupDelegates();

	Super::NativeDestruct();
}

void UOptionPopupWidget::ShowOptionPopup()
{
	bIsOpen = true;
	SetVisibility(ESlateVisibility::Visible);
}

void UOptionPopupWidget::HideOptionPopup()
{
	bIsOpen = false;
	SetVisibility(ESlateVisibility::Collapsed);
}

void UOptionPopupWidget::ToggleOptionPopup()
{
	const ESlateVisibility CurrentVisibility = GetVisibility();
	if (CurrentVisibility == ESlateVisibility::Collapsed || CurrentVisibility == ESlateVisibility::Hidden)
	{
		ShowOptionPopup();
		return;
	}

	HideOptionPopup();
}

void UOptionPopupWidget::SetPlayerHUD(APlayerHUD* InPlayerHUD)
{
	PlayerHUD = InPlayerHUD;
}

void UOptionPopupWidget::RequestExitToMain()
{
	if (!PlayerHUD)
	{
		return;
	}

	PlayerHUD->RequestExitToMain();
}

void UOptionPopupWidget::RequestBGMVolumeChanged(float Value)
{
	if (!PlayerHUD)
	{
		return;
	}

	PlayerHUD->RequestBGMVolumeChanged(Value);
}

void UOptionPopupWidget::RequestSFXVolumeChanged(float Value)
{
	if (!PlayerHUD)
	{
		return;
	}

	PlayerHUD->RequestSFXVolumeChanged(Value);
}

void UOptionPopupWidget::RequestVoiceChatEnabledChanged(bool bEnabled)
{
	if (!PlayerHUD)
	{
		return;
	}

	PlayerHUD->RequestVoiceChatEnabledChanged(bEnabled);
}

void UOptionPopupWidget::RequestResolutionChanged(const FString& Resolution)
{
	if (!PlayerHUD)
	{
		return;
	}

	PlayerHUD->RequestResolutionChanged(Resolution);
}

void UOptionPopupWidget::InitializeResolutionOptions()
{
	if (!ComboBox_Resolution)
	{
		return;
	}

	if (ComboBox_Resolution->GetOptionCount() == 0)
	{
		ComboBox_Resolution->AddOption(TEXT("1280x720"));
		ComboBox_Resolution->AddOption(TEXT("1600x900"));
		ComboBox_Resolution->AddOption(TEXT("1920x1080"));
		ComboBox_Resolution->AddOption(TEXT("2560x1440"));
	}

	if (ComboBox_Resolution->GetSelectedOption().IsEmpty() && ComboBox_Resolution->GetOptionCount() > 0)
	{
		ComboBox_Resolution->SetSelectedIndex(0);
	}

	SelectedResolution = ComboBox_Resolution->GetSelectedOption();
}

void UOptionPopupWidget::BindOptionPopupDelegates()
{
	if (Slider_BGM)
	{
		BGMValue = Slider_BGM->GetValue();
		Slider_BGM->OnValueChanged.AddUniqueDynamic(this, &UOptionPopupWidget::HandleBGMValueChanged);
	}

	if (Slider_SFX)
	{
		SFXValue = Slider_SFX->GetValue();
		Slider_SFX->OnValueChanged.AddUniqueDynamic(this, &UOptionPopupWidget::HandleSFXValueChanged);
	}

	if (CheckBox_Voice)
	{
		bIsVoiceChatEnabled = CheckBox_Voice->IsChecked();
		CheckBox_Voice->OnCheckStateChanged.AddUniqueDynamic(this, &UOptionPopupWidget::HandleVoiceChatCheckStateChanged);
	}

	if (ComboBox_Resolution)
	{
		ComboBox_Resolution->OnSelectionChanged.AddUniqueDynamic(this, &UOptionPopupWidget::HandleResolutionSelectionChanged);
	}

	if (Button_Close)
	{
		Button_Close->OnClicked.AddUniqueDynamic(this, &UOptionPopupWidget::HandleCloseButtonClicked);
	}
}

void UOptionPopupWidget::UnbindOptionPopupDelegates()
{
	if (Slider_BGM)
	{
		Slider_BGM->OnValueChanged.RemoveDynamic(this, &UOptionPopupWidget::HandleBGMValueChanged);
	}

	if (Slider_SFX)
	{
		Slider_SFX->OnValueChanged.RemoveDynamic(this, &UOptionPopupWidget::HandleSFXValueChanged);
	}

	if (CheckBox_Voice)
	{
		CheckBox_Voice->OnCheckStateChanged.RemoveDynamic(this, &UOptionPopupWidget::HandleVoiceChatCheckStateChanged);
	}

	if (ComboBox_Resolution)
	{
		ComboBox_Resolution->OnSelectionChanged.RemoveDynamic(this, &UOptionPopupWidget::HandleResolutionSelectionChanged);
	}

	if (Button_Close)
	{
		Button_Close->OnClicked.RemoveDynamic(this, &UOptionPopupWidget::HandleCloseButtonClicked);
	}
}

void UOptionPopupWidget::HandleBGMValueChanged(float Value)
{
	BGMValue = Value;
	RequestBGMVolumeChanged(Value);
}

void UOptionPopupWidget::HandleSFXValueChanged(float Value)
{
	SFXValue = Value;
	RequestSFXVolumeChanged(Value);
}

void UOptionPopupWidget::HandleVoiceChatCheckStateChanged(bool bIsChecked)
{
	bIsVoiceChatEnabled = bIsChecked;
	RequestVoiceChatEnabledChanged(bIsChecked);
}

void UOptionPopupWidget::HandleResolutionSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	SelectedResolution = SelectedItem;
	RequestResolutionChanged(SelectedResolution);
}

void UOptionPopupWidget::HandleCloseButtonClicked()
{
	RequestExitToMain();
}
