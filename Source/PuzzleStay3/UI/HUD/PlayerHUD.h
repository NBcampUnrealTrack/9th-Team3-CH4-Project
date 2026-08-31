#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

class UTextNotifyWidget;
class ULifeCountWidget;
class UInteractionNotifyWidget;
class UTimerNotifyWidget;
class UDoorOpenButtonWidget;
class UVoiceChatIconWidget;
class UOptionPopupWidget;
class UTitleWidget;
class UTutorialNotifyWidget;
class UGameOverWidget;
class UStage5RoleSelectWidget;
class UPS3ViewModel;

USTRUCT(BlueprintType)
struct FPS3HUDWidgets
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Widget")
	TObjectPtr<UTextNotifyWidget> TextNotifyWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Widget")
	TObjectPtr<ULifeCountWidget> LifeCountWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Widget")
	TObjectPtr<UInteractionNotifyWidget> InteractionNotifyWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Widget")
	TObjectPtr<UTimerNotifyWidget> TimerNotifyWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Widget")
	TObjectPtr<UDoorOpenButtonWidget> DoorOpenButtonWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Widget")
	TObjectPtr<UVoiceChatIconWidget> VoiceChatIconWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Widget")
	TObjectPtr<UOptionPopupWidget> OptionPopupWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Widget")
	TObjectPtr<UTitleWidget> TitleWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Widget")
	TObjectPtr<UTutorialNotifyWidget> TutorialNotifyWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Widget")
	TObjectPtr<UGameOverWidget> GameOverWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Widget")
	TObjectPtr<UStage5RoleSelectWidget> Stage5RoleSelectWidget;
};

UCLASS()
class PUZZLESTAY3_API APlayerHUD : public AHUD
{
	GENERATED_BODY()

public:
	APlayerHUD();

	UFUNCTION(BlueprintCallable, Category = "UI|Widget")
	void SetWidgets(const FPS3HUDWidgets& InWidgets);

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetViewModel(UPS3ViewModel* InViewModel);

	UFUNCTION(BlueprintPure, Category = "UI")
	UPS3ViewModel* GetViewModel() const;

	UFUNCTION(BlueprintCallable, Category = "UI|TextNotify")
	void ShowTextNotify(const FText& InDisplayText, float InFontSize, float InDisplayDuration);

	UFUNCTION(BlueprintCallable, Category = "UI|LifeCount")
	void UpdateLifeCount(int32 InCurrentLifeCount, int32 InMaxLifeCount);

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void ShowInteractionNotify(FName InNotifyId, const FText& InKeyName);

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void HideInteractionNotify(FName InNotifyId);

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void HideAllInteractionNotifies();

	UFUNCTION(BlueprintCallable, Category = "UI|TimerNotify")
	void UpdateTimerNotify(float InRemainingTime, float InTotalTime);

	UFUNCTION(BlueprintCallable, Category = "UI|TimerNotify")
	void HideTimerNotify();

	UFUNCTION(BlueprintCallable, Category = "UI|TutorialNotify")
	void ShowTutorialNotify();

	UFUNCTION(BlueprintCallable, Category = "UI|TutorialNotify")
	void HideTutorialNotify();

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
	UPROPERTY(BlueprintReadOnly, Category = "UI|Widget")
	FPS3HUDWidgets Widgets;

	UPROPERTY(BlueprintReadOnly, Category = "UI|ViewModel")
	TObjectPtr<UPS3ViewModel> ViewModel;

private:
	void ApplyViewModelToWidgets();
};
