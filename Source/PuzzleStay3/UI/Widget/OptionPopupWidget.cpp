#include "OptionPopupWidget.h"

#include "../HUD/PlayerHUD.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/Slider.h"
#include "Engine/Engine.h" // 현준 수정
#include "GameFramework/GameUserSettings.h" // 현준 수정

void UOptionPopupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeResolutionOptions();
	SyncDisplaySettingsToUI(); // 현준 수정
	BindOptionPopupDelegates();
}

void UOptionPopupWidget::NativeDestruct()
{
	UnbindOptionPopupDelegates();

	Super::NativeDestruct();
}

void UOptionPopupWidget::ShowOptionPopup()
{
	SyncDisplaySettingsToUI(); // 현준 수정
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
		ComboBox_Resolution->AddOption(TEXT("1280 × 720")); // 현준 수정
		ComboBox_Resolution->AddOption(TEXT("1600 × 900")); // 현준 수정
		ComboBox_Resolution->AddOption(TEXT("1920 × 1080")); // 현준 수정
		ComboBox_Resolution->AddOption(TEXT("2560 × 1440")); // 현준 수정
		ComboBox_Resolution->AddOption(TEXT("3840 × 2160")); // 현준 수정
	}

	if (ComboBox_Resolution->GetSelectedOption().IsEmpty() && ComboBox_Resolution->GetOptionCount() > 0)
	{
		ComboBox_Resolution->SetSelectedIndex(0);
	}

	SelectedResolution = ComboBox_Resolution->GetSelectedOption();
}

void UOptionPopupWidget::SyncDisplaySettingsToUI() // 현준 수정
{
	UGameUserSettings* GameUserSettings = GEngine ? GEngine->GetGameUserSettings() : nullptr; // 현준 수정
	if (!GameUserSettings) // 현준 수정
	{
		return; // 현준 수정
	}

	bIsUpdatingDisplaySettingsUI = true; // 현준 수정

	if (CheckBox_Voice) // 현준 수정
	{
		const EWindowMode::Type FullscreenMode = GameUserSettings->GetFullscreenMode(); // 현준 수정
		bIsVoiceChatEnabled = FullscreenMode == EWindowMode::Fullscreen; // 현준 수정
		CheckBox_Voice->SetIsChecked(bIsVoiceChatEnabled); // 현준 수정
	}

	if (ComboBox_Resolution) // 현준 수정
	{
		const FString ResolutionOption = MakeResolutionOptionText(GameUserSettings->GetScreenResolution()); // 현준 수정
		if (ComboBox_Resolution->FindOptionIndex(ResolutionOption) != INDEX_NONE) // 현준 수정
		{
			SelectedResolution = ResolutionOption; // 현준 수정
			ComboBox_Resolution->SetSelectedOption(ResolutionOption); // 현준 수정
		}
	}

	bIsUpdatingDisplaySettingsUI = false; // 현준 수정
}

void UOptionPopupWidget::ApplyFullscreenSetting(bool bFullscreen) // 현준 수정
{
	UGameUserSettings* GameUserSettings = GEngine ? GEngine->GetGameUserSettings() : nullptr; // 현준 수정
	if (!GameUserSettings) // 현준 수정
	{
		return; // 현준 수정
	}

	GameUserSettings->SetFullscreenMode(bFullscreen ? EWindowMode::Fullscreen : EWindowMode::Windowed); // 현준 수정
	GameUserSettings->ApplySettings(false); // 현준 수정
	GameUserSettings->SaveSettings(); // 현준 수정
}

void UOptionPopupWidget::ApplyResolutionSetting(const FString& Resolution) // 현준 수정
{
	UGameUserSettings* GameUserSettings = GEngine ? GEngine->GetGameUserSettings() : nullptr; // 현준 수정
	if (!GameUserSettings) // 현준 수정
	{
		return; // 현준 수정
	}

	FIntPoint ScreenResolution; // 현준 수정
	if (!TryResolveResolution(Resolution, ScreenResolution)) // 현준 수정
	{
		return; // 현준 수정
	}

	GameUserSettings->SetScreenResolution(ScreenResolution); // 현준 수정
	GameUserSettings->ApplySettings(false); // 현준 수정
	GameUserSettings->SaveSettings(); // 현준 수정
}

bool UOptionPopupWidget::TryResolveResolution(const FString& Resolution, FIntPoint& OutResolution) const // 현준 수정
{
	if (Resolution == TEXT("1280 × 720") || Resolution == TEXT("1280x720")) // 현준 수정
	{
		OutResolution = FIntPoint(1280, 720); // 현준 수정
		return true; // 현준 수정
	}

	if (Resolution == TEXT("1600 × 900") || Resolution == TEXT("1600x900")) // 현준 수정
	{
		OutResolution = FIntPoint(1600, 900); // 현준 수정
		return true; // 현준 수정
	}

	if (Resolution == TEXT("1920 × 1080") || Resolution == TEXT("1920x1080")) // 현준 수정
	{
		OutResolution = FIntPoint(1920, 1080); // 현준 수정
		return true; // 현준 수정
	}

	if (Resolution == TEXT("2560 × 1440") || Resolution == TEXT("2560x1440")) // 현준 수정
	{
		OutResolution = FIntPoint(2560, 1440); // 현준 수정
		return true; // 현준 수정
	}

	if (Resolution == TEXT("3840 × 2160") || Resolution == TEXT("3840x2160")) // 현준 수정
	{
		OutResolution = FIntPoint(3840, 2160); // 현준 수정
		return true; // 현준 수정
	}

	return false; // 현준 수정
}

FString UOptionPopupWidget::MakeResolutionOptionText(const FIntPoint& Resolution) const // 현준 수정
{
	return FString::Printf(TEXT("%d × %d"), Resolution.X, Resolution.Y); // 현준 수정
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
	if (bIsUpdatingDisplaySettingsUI) // 현준 수정
	{
		return; // 현준 수정
	}

	bIsVoiceChatEnabled = bIsChecked;
	ApplyFullscreenSetting(bIsChecked); // 현준 수정
}

void UOptionPopupWidget::HandleResolutionSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (bIsUpdatingDisplaySettingsUI) // 현준 수정
	{
		return; // 현준 수정
	}

	SelectedResolution = SelectedItem;
	ApplyResolutionSetting(SelectedResolution); // 현준 수정
}

void UOptionPopupWidget::HandleCloseButtonClicked()
{
	RequestExitToMain();
}
