#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DoorOpenButtonWidget.generated.h"

class APlayerHUD;
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
	void UpdateDoorOpenButtons(
		bool bInDoor1Unlocked,
		bool bInDoor2Unlocked,
		bool bInDoor3Unlocked,
		bool bInDoor4Unlocked
	);

	UFUNCTION(BlueprintCallable, Category = "UI|DoorOpenButton")
	void RequestOpenDoor(int32 InDoorIndex);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "UI|ViewModel")
	TObjectPtr<UPS3ViewModel> ViewModel;

	UPROPERTY(BlueprintReadOnly, Category = "UI|HUD")
	TObjectPtr<APlayerHUD> PlayerHUD;
};
