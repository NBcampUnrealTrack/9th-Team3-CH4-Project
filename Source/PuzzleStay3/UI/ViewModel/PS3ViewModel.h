#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/PS3InteractionNotifyType.h"
#include "Data/Enum/PS3PlayerRole.h"
#include "Data/Enum/PS3TextNotifyType.h"
#include "MVVMViewModelBase.h"
#include "PS3ViewModel.generated.h"

class APlayerHUD;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDoorActivationRequested_UI, int32, DoorIndex, bool, bIsActive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStage5RoleSelectionRequested_UI, EPS3PlayerRole, SelectedRole);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExitToMainRequested_UI);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBGMVolumeChanged_UI, float, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSFXVolumeChanged_UI, float, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVoiceChatEnabledChanged_UI, bool, bEnabled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResolutionChanged_UI, const FString&, Resolution);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStartRequested_UI);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameExitRequested_UI);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameRestartRequested_UI);

UCLASS(BlueprintType)
class PUZZLESTAY3_API UPS3ViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	void SetPlayerHUD(APlayerHUD* InPlayerHUD);

	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = "UI|TextNotify")
	void RequestTextNotify(EPS3TextNotifyType NotifyType);

	UFUNCTION(BlueprintCallable, Category = "UI|TextNotify")
	void RequestSetTextNotifyVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "UI|LifeCount")
	void RequestSetLifeCountVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "UI|LifeCount")
	void RequestUpdateLifeCount(int32 InCurrentLifeCount, int32 InMaxLifeCount);

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void RequestSetInteractionNotifyVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void RequestShowInteractionNotify(EPS3InteractionNotifyType NotifyType);

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void RequestHideInteractionNotify(EPS3InteractionNotifyType NotifyType);

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void RequestHideAllInteractionNotifies();

	UFUNCTION(BlueprintCallable, Category = "UI|TimerNotify")
	void RequestSetTimerNotifyVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "UI|TimerNotify")
	void RequestTimerNotify(FName InTimerId, float InDuration);

	UFUNCTION(BlueprintCallable, Category = "UI|TimerNotify")
	void RequestReduceTimerNotify(FName InTimerId, float InReduceTime);

	UFUNCTION(BlueprintCallable, Category = "UI|TimerNotify")
	void RequestResetTimerNotify();

	UFUNCTION(BlueprintCallable, Category = "UI|TutorialNotify")
	void RequestSetTutorialNotifyVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "UI|DoorOpenButton")
	void RequestSetDoorOpenButtonVisible(bool bVisible);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	int32 GetCurrentLifeCount() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetCurrentLifeCount(int32 InCurrentLifeCount);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	int32 GetMaxLifeCount() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetMaxLifeCount(int32 InMaxLifeCount);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	TArray<FText> GetInteractionKeyNames() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetInteractionKeyNames(const TArray<FText>& InInteractionKeyNames);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	bool GetIsInteractionNotifyVisible() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetIsInteractionNotifyVisible(bool bInIsInteractionNotifyVisible);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	bool GetIsDoor1Unlocked() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetIsDoor1Unlocked(bool bInIsDoor1Unlocked);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	bool GetIsDoor2Unlocked() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetIsDoor2Unlocked(bool bInIsDoor2Unlocked);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	bool GetIsDoor3Unlocked() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetIsDoor3Unlocked(bool bInIsDoor3Unlocked);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	bool GetIsDoor4Unlocked() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetIsDoor4Unlocked(bool bInIsDoor4Unlocked);

	UFUNCTION(BlueprintCallable, Category = "UI|DoorOpenButton")
	void RequestDoorActivation(int32 InDoorIndex, bool bIsActive);

	UPROPERTY(BlueprintAssignable, Category = "UI|DoorOpenButton")
	FOnDoorActivationRequested_UI OnDoorActivationRequested_UI;

	UFUNCTION(BlueprintCallable, Category = "UI|VoiceChatIcon")
	void RequestVoiceChatSpeaking(bool bInIsSpeaking);

	UFUNCTION(BlueprintCallable, Category = "UI|VoiceChatIcon")
	void RequestSetVoiceChatIconVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void RequestSetOptionPopupVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void RequestToggleOptionPopup();

	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void RequestExitToMain();

	UPROPERTY(BlueprintAssignable, Category = "UI|OptionPopup")
	FOnExitToMainRequested_UI OnExitToMainRequested_UI;

	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void RequestBGMVolumeChanged(float Value);

	UPROPERTY(BlueprintAssignable, Category = "UI|OptionPopup")
	FOnBGMVolumeChanged_UI OnBGMVolumeChanged_UI;

	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void RequestSFXVolumeChanged(float Value);

	UPROPERTY(BlueprintAssignable, Category = "UI|OptionPopup")
	FOnSFXVolumeChanged_UI OnSFXVolumeChanged_UI;

	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void RequestVoiceChatEnabledChanged(bool bEnabled);

	UPROPERTY(BlueprintAssignable, Category = "UI|OptionPopup")
	FOnVoiceChatEnabledChanged_UI OnVoiceChatEnabledChanged_UI;

	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void RequestResolutionChanged(const FString& Resolution);

	UPROPERTY(BlueprintAssignable, Category = "UI|OptionPopup")
	FOnResolutionChanged_UI OnResolutionChanged_UI;

	UFUNCTION(BlueprintCallable, Category = "UI|Title")
	void RequestSetTitleVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "UI|Title")
	void RequestGameStart();

	UPROPERTY(BlueprintAssignable, Category = "UI|Title")
	FOnGameStartRequested_UI OnGameStartRequested_UI;

	UFUNCTION(BlueprintCallable, Category = "UI|Title")
	void RequestGameExit();

	UPROPERTY(BlueprintAssignable, Category = "UI|Title")
	FOnGameExitRequested_UI OnGameExitRequested_UI;

	UFUNCTION(BlueprintCallable, Category = "UI|GameOver")
	void RequestSetGameOverVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "UI|GameOver")
	void RequestGameRestart();

	UPROPERTY(BlueprintAssignable, Category = "UI|GameOver")
	FOnGameRestartRequested_UI OnGameRestartRequested_UI;

	UFUNCTION(BlueprintCallable, Category = "UI|Stage5RoleSelect")
	void RequestSetStage5RoleSelectVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "UI|Stage5RoleSelect")
	void RequestStage5RoleSelection(EPS3PlayerRole SelectedRole);

	UPROPERTY(BlueprintAssignable, Category = "UI|Stage5RoleSelect")
	FOnStage5RoleSelectionRequested_UI OnStage5RoleSelectionRequested_UI;

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	bool GetIsOpen() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetIsOpen(bool bInIsOpen);

private:
	void BindRoleSelectionUIDelegate();
	void UnbindRoleSelectionUIDelegate();
	void HandleRoleSelection_UI(bool bVisible);

	UPROPERTY(BlueprintReadOnly, Category = "UI|HUD", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<APlayerHUD> PlayerHUD;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	int32 CurrentLifeCount = 0;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	int32 MaxLifeCount = 0;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	TArray<FText> InteractionKeyNames;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter = "GetIsInteractionNotifyVisible", Setter = "SetIsInteractionNotifyVisible", Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	bool bIsInteractionNotifyVisible = false;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter = "GetIsDoor1Unlocked", Setter = "SetIsDoor1Unlocked", Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	bool bIsDoor1Unlocked = false;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter = "GetIsDoor2Unlocked", Setter = "SetIsDoor2Unlocked", Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	bool bIsDoor2Unlocked = false;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter = "GetIsDoor3Unlocked", Setter = "SetIsDoor3Unlocked", Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	bool bIsDoor3Unlocked = false;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter = "GetIsDoor4Unlocked", Setter = "SetIsDoor4Unlocked", Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	bool bIsDoor4Unlocked = false;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter = "GetIsOpen", Setter = "SetIsOpen", Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	bool bIsOpen = false;
};
