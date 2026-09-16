#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ComboBoxString.h"
#include "OptionPopupWidget.generated.h"

class UButton;
class UCheckBox;
class UComboBoxString;
class USlider;
class APlayerHUD;

UCLASS()
class PUZZLESTAY3_API UOptionPopupWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void ShowOptionPopup();

	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void HideOptionPopup();

	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void ToggleOptionPopup();

	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	void SetPlayerHUD(APlayerHUD* InPlayerHUD);

	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void RequestExitToMain();

	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void RequestBGMVolumeChanged(float Value);

	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void RequestSFXVolumeChanged(float Value);

	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void RequestVoiceChatEnabledChanged(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void RequestResolutionChanged(const FString& Resolution);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|OptionPopup")
	bool bIsOpen = false;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USlider> Slider_BGM;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USlider> Slider_SFX;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCheckBox> CheckBox_Voice;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UComboBoxString> ComboBox_Resolution;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> Button_Close;

	UPROPERTY(BlueprintReadOnly, Category = "UI|OptionPopup")
	float BGMValue = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "UI|OptionPopup")
	float SFXValue = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "UI|OptionPopup")
	bool bIsVoiceChatEnabled = true;

	UPROPERTY(BlueprintReadOnly, Category = "UI|OptionPopup")
	FString SelectedResolution;

	UPROPERTY(BlueprintReadOnly, Category = "UI|HUD")
	TObjectPtr<APlayerHUD> PlayerHUD;

private:
	void InitializeResolutionOptions();
	void SyncDisplaySettingsToUI(); // 현준 수정
	void ApplyFullscreenSetting(bool bFullscreen); // 현준 수정
	void ApplyResolutionSetting(const FString& Resolution); // 현준 수정
	bool TryResolveResolution(const FString& Resolution, FIntPoint& OutResolution) const; // 현준 수정
	FString MakeResolutionOptionText(const FIntPoint& Resolution) const; // 현준 수정
	void BindOptionPopupDelegates();
	void UnbindOptionPopupDelegates();

	bool bIsUpdatingDisplaySettingsUI = false; // 현준 수정

	UFUNCTION()
	void HandleBGMValueChanged(float Value);

	UFUNCTION()
	void HandleSFXValueChanged(float Value);

	UFUNCTION()
	void HandleVoiceChatCheckStateChanged(bool bIsChecked);

	UFUNCTION()
	void HandleResolutionSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void HandleCloseButtonClicked();
};
