#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"

class APlayerHUD;
class UButton;

UCLASS()
class PUZZLESTAY3_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	void SetPlayerHUD(APlayerHUD* InPlayerHUD);

	UFUNCTION(BlueprintCallable, Category = "UI|GameOver")
	void ShowGameOver();

	UFUNCTION(BlueprintCallable, Category = "UI|GameOver")
	void HideGameOver();

	UFUNCTION(BlueprintCallable, Category = "UI|GameOver")
	void RequestGameRestart();

	UFUNCTION(BlueprintCallable, Category = "UI|GameOver")
	void RequestExitToMain();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "UI|HUD")
	TObjectPtr<APlayerHUD> PlayerHUD;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_Restart;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_ExitToMain;

private:
	void BindGameOverButtonDelegates();
	void UnbindGameOverButtonDelegates();

	UFUNCTION()
	void HandleRestartButtonClicked();

	UFUNCTION()
	void HandleExitToMainButtonClicked();
};
