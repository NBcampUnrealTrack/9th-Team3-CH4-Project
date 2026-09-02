#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ComboBoxString.h"
#include "OptionPopupWidget.generated.h"

class UButton;
class UCheckBox;
class UComboBoxString;
class USlider;

UCLASS()
class PUZZLESTAY3_API UOptionPopupWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void ShowOptionPopup();

	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void HideOptionPopup();

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

private:
	void InitializeResolutionOptions();
	void BindOptionPopupDelegates();
	void UnbindOptionPopupDelegates();

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
