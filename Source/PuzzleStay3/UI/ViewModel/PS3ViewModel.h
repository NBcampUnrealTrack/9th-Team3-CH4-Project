#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/PS3PlayerRole.h"
#include "MVVMViewModelBase.h"
#include "PS3ViewModel.generated.h"

class APlayerHUD;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDoorActivationRequested, int32, DoorIndex, bool, bIsActive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStage5RoleSelectionRequested, EPS3PlayerRole, SelectedRole);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExitToMainRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBGMVolumeChanged, float, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSFXVolumeChanged, float, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVoiceChatEnabledChanged, bool, bEnabled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResolutionChanged, const FString&, Resolution);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStartRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameExitRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameRestartRequested);

UCLASS(BlueprintType)
class PUZZLESTAY3_API UPS3ViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	void SetPlayerHUD(APlayerHUD* InPlayerHUD);

	UFUNCTION(BlueprintCallable, Category = "UI|TextNotify")
	void RequestTextNotify(const FText& InDisplayText, float InFontSize, float InDisplayDuration);

	UFUNCTION(BlueprintCallable, Category = "UI|TextNotify")
	void RequestHideTextNotify();

	UFUNCTION(BlueprintCallable, Category = "UI|LifeCount")
	void RequestShowLifeCount();

	UFUNCTION(BlueprintCallable, Category = "UI|LifeCount")
	void RequestHideLifeCount();

	UFUNCTION(BlueprintCallable, Category = "UI|LifeCount")
	void RequestUpdateLifeCount(int32 InCurrentLifeCount, int32 InMaxLifeCount);

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void RequestShowInteractionNotifyWidget();

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void RequestHideInteractionNotifyWidget();

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void RequestShowInteractionNotify(FName InNotifyId, const FText& InKeyName);

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void RequestHideInteractionNotify(FName InNotifyId);

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void RequestHideAllInteractionNotifies();

	UFUNCTION(BlueprintCallable, Category = "UI|TimerNotify")
	void RequestShowTimerNotify();

	UFUNCTION(BlueprintCallable, Category = "UI|TimerNotify")
	void RequestHideTimerNotifyWidget();

	UFUNCTION(BlueprintCallable, Category = "UI|TimerNotify")
	void RequestTimerNotify(float InDuration);

	UFUNCTION(BlueprintCallable, Category = "UI|TimerNotify")
	void RequestHideTimerNotify();

	UFUNCTION(BlueprintCallable, Category = "UI|TutorialNotify")
	void RequestShowTutorialNotify();

	UFUNCTION(BlueprintCallable, Category = "UI|TutorialNotify")
	void RequestHideTutorialNotify();

	UFUNCTION(BlueprintCallable, Category = "UI|DoorOpenButton")
	void RequestShowDoorOpenButton();

	UFUNCTION(BlueprintCallable, Category = "UI|DoorOpenButton")
	void RequestHideDoorOpenButton();

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
	FOnDoorActivationRequested OnDoorActivationRequested;

	UFUNCTION(BlueprintCallable, Category = "UI|VoiceChatIcon")
	void RequestVoiceChatSpeaking(bool bInIsSpeaking);

	UFUNCTION(BlueprintCallable, Category = "UI|VoiceChatIcon")
	void RequestShowVoiceChatIcon();

	UFUNCTION(BlueprintCallable, Category = "UI|VoiceChatIcon")
	void RequestHideVoiceChatIcon();

	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void RequestShowOptionPopup();

	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void RequestHideOptionPopup();

	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void RequestToggleOptionPopup();

	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void RequestExitToMain();

	UPROPERTY(BlueprintAssignable, Category = "UI|OptionPopup")
	FOnExitToMainRequested OnExitToMainRequested;

	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void RequestBGMVolumeChanged(float Value);

	UPROPERTY(BlueprintAssignable, Category = "UI|OptionPopup")
	FOnBGMVolumeChanged OnBGMVolumeChanged;

	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void RequestSFXVolumeChanged(float Value);

	UPROPERTY(BlueprintAssignable, Category = "UI|OptionPopup")
	FOnSFXVolumeChanged OnSFXVolumeChanged;

	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void RequestVoiceChatEnabledChanged(bool bEnabled);

	UPROPERTY(BlueprintAssignable, Category = "UI|OptionPopup")
	FOnVoiceChatEnabledChanged OnVoiceChatEnabledChanged;

	UFUNCTION(BlueprintCallable, Category = "UI|OptionPopup")
	void RequestResolutionChanged(const FString& Resolution);

	UPROPERTY(BlueprintAssignable, Category = "UI|OptionPopup")
	FOnResolutionChanged OnResolutionChanged;

	UFUNCTION(BlueprintCallable, Category = "UI|Title")
	void RequestShowTitle();

	UFUNCTION(BlueprintCallable, Category = "UI|Title")
	void RequestHideTitle();

	UFUNCTION(BlueprintCallable, Category = "UI|Title")
	void RequestGameStart();

	UPROPERTY(BlueprintAssignable, Category = "UI|Title")
	FOnGameStartRequested OnGameStartRequested;

	UFUNCTION(BlueprintCallable, Category = "UI|Title")
	void RequestGameExit();

	UPROPERTY(BlueprintAssignable, Category = "UI|Title")
	FOnGameExitRequested OnGameExitRequested;

	UFUNCTION(BlueprintCallable, Category = "UI|GameOver")
	void RequestShowGameOver();

	UFUNCTION(BlueprintCallable, Category = "UI|GameOver")
	void RequestHideGameOver();

	UFUNCTION(BlueprintCallable, Category = "UI|GameOver")
	void RequestGameRestart();

	UPROPERTY(BlueprintAssignable, Category = "UI|GameOver")
	FOnGameRestartRequested OnGameRestartRequested;

	UFUNCTION(BlueprintCallable, Category = "UI|Stage5RoleSelect")
	void RequestShowStage5RoleSelect();

	UFUNCTION(BlueprintCallable, Category = "UI|Stage5RoleSelect")
	void RequestHideStage5RoleSelect();

	UFUNCTION(BlueprintCallable, Category = "UI|Stage5RoleSelect")
	void RequestStage5RoleSelection(EPS3PlayerRole SelectedRole);

	UPROPERTY(BlueprintAssignable, Category = "UI|Stage5RoleSelect")
	FOnStage5RoleSelectionRequested OnStage5RoleSelectionRequested;

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	bool GetIsOpen() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetIsOpen(bool bInIsOpen);

private:
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
