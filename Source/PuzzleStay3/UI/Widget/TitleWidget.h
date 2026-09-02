#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleWidget.generated.h"

class APlayerHUD;
class UButton;

UCLASS()
class PUZZLESTAY3_API UTitleWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	void SetPlayerHUD(APlayerHUD* InPlayerHUD);

	UFUNCTION(BlueprintCallable, Category = "UI|Title")
	void ShowTitle();

	UFUNCTION(BlueprintCallable, Category = "UI|Title")
	void HideTitle();

	UFUNCTION(BlueprintCallable, Category = "UI|Title")
	void RequestGameStart();

	UFUNCTION(BlueprintCallable, Category = "UI|Title")
	void RequestTitleOption();

	UFUNCTION(BlueprintCallable, Category = "UI|Title")
	void RequestGameExit();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "UI|HUD")
	TObjectPtr<APlayerHUD> PlayerHUD;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_GameStart;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_Setting;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_GameExit;

private:
	void BindTitleButtonDelegates();
	void UnbindTitleButtonDelegates();

	UFUNCTION()
	void HandleGameStartButtonClicked();

	UFUNCTION()
	void HandleSettingButtonClicked();

	UFUNCTION()
	void HandleGameExitButtonClicked();
};
