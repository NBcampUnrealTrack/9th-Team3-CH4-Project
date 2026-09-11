#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/PS3InteractionNotifyType.h"
#include "Data/Enum/PS3PlayerRole.h"
#include "Data/Enum/PS3TextNotifyType.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

class UTextNotifyWidget;
class UPS3RootHUDWidget;
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

	UFUNCTION(BlueprintPure, Category = "UI")
	bool IsUIReady() const;

	UFUNCTION(BlueprintCallable, Category = "UI|TextNotify")
	void ShowTextNotify(EPS3TextNotifyType NotifyType);

	UFUNCTION(BlueprintCallable, Category = "UI|TextNotify")
	void SetTextNotifyVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "UI|LifeCount")
	void SetLifeCountVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "UI|LifeCount")
	void UpdateLifeCount(int32 InCurrentLifeCount, int32 InMaxLifeCount);

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void SetInteractionNotifyVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void ShowInteractionNotify(EPS3InteractionNotifyType NotifyType);

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void HideInteractionNotify(EPS3InteractionNotifyType NotifyType);

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void HideAllInteractionNotifies();

	UFUNCTION(BlueprintCallable, Category = "UI|TimerNotify")
	void SetTimerNotifyVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "UI|TimerNotify")
	void UpdateTimerNotify(FName InTimerId, float InMaxTime, float InCurrentTime);

	UFUNCTION(BlueprintCallable, Category = "UI|TimerNotify")
	void ReduceTimerNotify(FName InTimerId, float InReduceTime);

	UFUNCTION(BlueprintCallable, Category = "UI|TimerNotify")
	void ResetTimerNotify();

	UFUNCTION(BlueprintCallable, Category = "UI|TutorialNotify")
	void SetTutorialNotifyVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "UI|DoorOpenButton")
	void SetDoorOpenButtonVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "UI|DoorOpenButton")
	void UpdateDoorOpenButtons(
		bool bInDoor1Unlocked,
		bool bInDoor2Unlocked,
		bool bInDoor3Unlocked,
		bool bInDoor4Unlocked
	);

	UFUNCTION(BlueprintCallable, Category = "UI|DoorOpenButton")
	void RequestDoorActivation(int32 InDoorIndex, bool bIsActive);

	UFUNCTION(BlueprintCallable, Category = "UI|VoiceChatIcon")
	void UpdateVoiceChatIcon(bool bInIsSpeaking);

	UFUNCTION(BlueprintCallable, Category = "UI|VoiceChatIcon")
	void SetVoiceChatIconVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void SetOptionPopupVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void ToggleOptionPopup();

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

	UFUNCTION(BlueprintCallable, Category = "UI|Title")
	void SetTitleVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "UI|Title")
	void RequestGameStart();

	UFUNCTION(BlueprintCallable, Category = "UI|Title")
	void RequestTitleOption();

	UFUNCTION(BlueprintCallable, Category = "UI|Title")
	void RequestGameExit();

	UFUNCTION(BlueprintCallable, Category = "UI|GameOver")
	void SetGameOverVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "UI|GameOver")
	void RequestGameRestart();

	UFUNCTION(BlueprintCallable, Category = "UI|Stage5RoleSelect")
	void SetStage5RoleSelectVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "UI|Stage5RoleSelect")
	void RequestStage5RoleSelection(EPS3PlayerRole SelectedRole);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|Widget")
	TSubclassOf<UPS3RootHUDWidget> RootHUDWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "UI|Widget")
	TObjectPtr<UPS3RootHUDWidget> RootHUDWidget;

	UPROPERTY(BlueprintReadOnly, Category = "UI|Widget")
	FPS3HUDWidgets Widgets;

	UPROPERTY(BlueprintReadOnly, Category = "UI|ViewModel")
	TObjectPtr<UPS3ViewModel> ViewModel;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	bool bIsUIReady = false;

private:
	void ApplyViewModelToWidgets();
};
