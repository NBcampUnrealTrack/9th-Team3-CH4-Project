#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DoorOpenButtonWidget.generated.h"

class APlayerHUD;
class UButton;
class UImage;
class UPS3ViewModel;

UCLASS()
class PUZZLESTAY3_API UDoorOpenButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetViewModel(UPS3ViewModel* InViewModel);

	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	void SetPlayerHUD(APlayerHUD* InPlayerHUD);

	UFUNCTION(BlueprintCallable, Category = "UI|DoorOpenButton")
	void ShowDoorOpenButton();

	UFUNCTION(BlueprintCallable, Category = "UI|DoorOpenButton")
	void HideDoorOpenButton();

	UFUNCTION(BlueprintCallable, Category = "UI|DoorOpenButton")
	void UpdateDoorOpenButtons(
		bool bInDoor1Unlocked,
		bool bInDoor2Unlocked,
		bool bInDoor3Unlocked,
		bool bInDoor4Unlocked
	);

	UFUNCTION(BlueprintCallable, Category = "UI|DoorOpenButton")
	void SetDoorPressedFilters(
		bool bDoor1Pressed,
		bool bDoor2Pressed,
		bool bDoor3Pressed,
		bool bDoor4Pressed
	);

	UFUNCTION(BlueprintCallable, Category = "UI|DoorOpenButton")
	void RequestDoorActivation(int32 InDoorIndex, bool bIsActive);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "UI|ViewModel")
	TObjectPtr<UPS3ViewModel> ViewModel;

	UPROPERTY(BlueprintReadOnly, Category = "UI|HUD")
	TObjectPtr<APlayerHUD> PlayerHUD;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> DoorButton1;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> DoorButton2;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> DoorButton3;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> DoorButton4;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UImage> DoorButton1PressedFilter;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UImage> DoorButton2PressedFilter;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UImage> DoorButton3PressedFilter;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UImage> DoorButton4PressedFilter;

private:
	void SetPressedFilterVisible(UImage* PressedFilter, bool bVisible);
	void BindDoorButtonDelegates();
	void UnbindDoorButtonDelegates();

	UFUNCTION()
	void HandleDoorButton1Pressed();

	UFUNCTION()
	void HandleDoorButton1Released();

	UFUNCTION()
	void HandleDoorButton2Pressed();

	UFUNCTION()
	void HandleDoorButton2Released();

	UFUNCTION()
	void HandleDoorButton3Pressed();

	UFUNCTION()
	void HandleDoorButton3Released();

	UFUNCTION()
	void HandleDoorButton4Pressed();

	UFUNCTION()
	void HandleDoorButton4Released();
};
