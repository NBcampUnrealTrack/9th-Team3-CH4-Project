#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/Enum/PS3PlayerRole.h"
#include "Stage5RoleSelectWidget.generated.h"

class APlayerHUD;
class UButton;

UCLASS()
class PUZZLESTAY3_API UStage5RoleSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	void SetPlayerHUD(APlayerHUD* InPlayerHUD);

	UFUNCTION(BlueprintCallable, Category = "UI|Stage5RoleSelect")
	void ShowStage5RoleSelect();

	UFUNCTION(BlueprintCallable, Category = "UI|Stage5RoleSelect")
	void HideStage5RoleSelect();

	UFUNCTION(BlueprintCallable, Category = "UI|Stage5RoleSelect")
	void RequestStage5RoleSelection(EPS3PlayerRole SelectedRole);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "UI|HUD")
	TObjectPtr<APlayerHUD> PlayerHUD;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> Button_Field;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> Button_Screen;

private:
	void BindStage5RoleButtonDelegates();
	void UnbindStage5RoleButtonDelegates();

	UFUNCTION()
	void HandleFieldButtonClicked();

	UFUNCTION()
	void HandleScreenButtonClicked();
};
