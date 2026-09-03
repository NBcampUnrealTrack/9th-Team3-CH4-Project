#include "PS3ViewModel.h"

#include "../HUD/PlayerHUD.h"

void UPS3ViewModel::SetPlayerHUD(APlayerHUD* InPlayerHUD)
{
	PlayerHUD = InPlayerHUD;
}

void UPS3ViewModel::RequestTextNotify(const FText& InDisplayText, float InFontSize, float InDisplayDuration)
{
	if (PlayerHUD)
	{
		PlayerHUD->ShowTextNotify(InDisplayText, InFontSize, InDisplayDuration);
	}
}

void UPS3ViewModel::RequestHideTextNotify()
{
	if (PlayerHUD)
	{
		PlayerHUD->HideTextNotify();
	}
}

void UPS3ViewModel::RequestShowLifeCount()
{
	if (PlayerHUD)
	{
		PlayerHUD->ShowLifeCount();
	}
}

void UPS3ViewModel::RequestHideLifeCount()
{
	if (PlayerHUD)
	{
		PlayerHUD->HideLifeCount();
	}
}

void UPS3ViewModel::RequestUpdateLifeCount(int32 InCurrentLifeCount, int32 InMaxLifeCount)
{
	if (PlayerHUD)
	{
		PlayerHUD->UpdateLifeCount(InCurrentLifeCount, InMaxLifeCount);
	}
}

void UPS3ViewModel::RequestShowInteractionNotifyWidget()
{
	if (PlayerHUD)
	{
		PlayerHUD->ShowInteractionNotifyWidget();
	}
}

void UPS3ViewModel::RequestHideInteractionNotifyWidget()
{
	if (PlayerHUD)
	{
		PlayerHUD->HideInteractionNotifyWidget();
	}
}

void UPS3ViewModel::RequestShowInteractionNotify(FName InNotifyId, const FText& InKeyName)
{
	if (PlayerHUD)
	{
		PlayerHUD->ShowInteractionNotify(InNotifyId, InKeyName);
	}
}

void UPS3ViewModel::RequestHideInteractionNotify(FName InNotifyId)
{
	if (PlayerHUD)
	{
		PlayerHUD->HideInteractionNotify(InNotifyId);
	}
}

void UPS3ViewModel::RequestHideAllInteractionNotifies()
{
	if (PlayerHUD)
	{
		PlayerHUD->HideAllInteractionNotifies();
	}
}

void UPS3ViewModel::RequestShowTimerNotify()
{
	if (PlayerHUD)
	{
		PlayerHUD->ShowTimerNotify();
	}
}

void UPS3ViewModel::RequestHideTimerNotifyWidget()
{
	if (PlayerHUD)
	{
		PlayerHUD->HideTimerNotifyWidget();
	}
}

void UPS3ViewModel::RequestTimerNotify(float InDuration)
{
	if (PlayerHUD)
	{
		PlayerHUD->UpdateTimerNotify(InDuration);
	}
}

void UPS3ViewModel::RequestHideTimerNotify()
{
	if (PlayerHUD)
	{
		PlayerHUD->HideTimerNotify();
	}
}

void UPS3ViewModel::RequestShowTutorialNotify()
{
	if (PlayerHUD)
	{
		PlayerHUD->ShowTutorialNotify();
	}
}

void UPS3ViewModel::RequestHideTutorialNotify()
{
	if (PlayerHUD)
	{
		PlayerHUD->HideTutorialNotify();
	}
}

void UPS3ViewModel::RequestShowDoorOpenButton()
{
	if (PlayerHUD)
	{
		PlayerHUD->ShowDoorOpenButton();
	}
}

void UPS3ViewModel::RequestHideDoorOpenButton()
{
	if (PlayerHUD)
	{
		PlayerHUD->HideDoorOpenButton();
	}
}

int32 UPS3ViewModel::GetCurrentLifeCount() const
{
	return CurrentLifeCount;
}

void UPS3ViewModel::SetCurrentLifeCount(int32 InCurrentLifeCount)
{
	UE_MVVM_SET_PROPERTY_VALUE(CurrentLifeCount, InCurrentLifeCount);
}

int32 UPS3ViewModel::GetMaxLifeCount() const
{
	return MaxLifeCount;
}

void UPS3ViewModel::SetMaxLifeCount(int32 InMaxLifeCount)
{
	UE_MVVM_SET_PROPERTY_VALUE(MaxLifeCount, InMaxLifeCount);
}

TArray<FText> UPS3ViewModel::GetInteractionKeyNames() const
{
	return InteractionKeyNames;
}

void UPS3ViewModel::SetInteractionKeyNames(const TArray<FText>& InInteractionKeyNames)
{
	UE_MVVM_SET_PROPERTY_VALUE(InteractionKeyNames, InInteractionKeyNames);
}

bool UPS3ViewModel::GetIsInteractionNotifyVisible() const
{
	return bIsInteractionNotifyVisible;
}

void UPS3ViewModel::SetIsInteractionNotifyVisible(bool bInIsInteractionNotifyVisible)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsInteractionNotifyVisible, bInIsInteractionNotifyVisible);
}

bool UPS3ViewModel::GetIsDoor1Unlocked() const
{
	return bIsDoor1Unlocked;
}

void UPS3ViewModel::SetIsDoor1Unlocked(bool bInIsDoor1Unlocked)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsDoor1Unlocked, bInIsDoor1Unlocked);
}

bool UPS3ViewModel::GetIsDoor2Unlocked() const
{
	return bIsDoor2Unlocked;
}

void UPS3ViewModel::SetIsDoor2Unlocked(bool bInIsDoor2Unlocked)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsDoor2Unlocked, bInIsDoor2Unlocked);
}

bool UPS3ViewModel::GetIsDoor3Unlocked() const
{
	return bIsDoor3Unlocked;
}

void UPS3ViewModel::SetIsDoor3Unlocked(bool bInIsDoor3Unlocked)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsDoor3Unlocked, bInIsDoor3Unlocked);
}

bool UPS3ViewModel::GetIsDoor4Unlocked() const
{
	return bIsDoor4Unlocked;
}

void UPS3ViewModel::SetIsDoor4Unlocked(bool bInIsDoor4Unlocked)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsDoor4Unlocked, bInIsDoor4Unlocked);
}

void UPS3ViewModel::RequestDoorActivation(int32 InDoorIndex, bool bIsActive)
{
	if (InDoorIndex < 1 || InDoorIndex > 4)
	{
		return;
	}

	OnDoorActivationRequested.Broadcast(InDoorIndex, bIsActive);
}

void UPS3ViewModel::RequestVoiceChatSpeaking(bool bInIsSpeaking)
{
	if (PlayerHUD)
	{
		PlayerHUD->UpdateVoiceChatIcon(bInIsSpeaking);
	}
}

void UPS3ViewModel::RequestShowVoiceChatIcon()
{
	if (PlayerHUD)
	{
		PlayerHUD->ShowVoiceChatIcon();
	}
}

void UPS3ViewModel::RequestHideVoiceChatIcon()
{
	if (PlayerHUD)
	{
		PlayerHUD->HideVoiceChatIcon();
	}
}

void UPS3ViewModel::RequestShowOptionPopup()
{
	if (PlayerHUD)
	{
		PlayerHUD->ShowOptionPopup();
	}
}

void UPS3ViewModel::RequestHideOptionPopup()
{
	if (PlayerHUD)
	{
		PlayerHUD->HideOptionPopup();
	}
}

void UPS3ViewModel::RequestToggleOptionPopup()
{
	if (PlayerHUD)
	{
		PlayerHUD->ToggleOptionPopup();
	}
}

void UPS3ViewModel::RequestExitToMain()
{
	OnExitToMainRequested.Broadcast();
}

void UPS3ViewModel::RequestBGMVolumeChanged(float Value)
{
	OnBGMVolumeChanged.Broadcast(Value);
}

void UPS3ViewModel::RequestSFXVolumeChanged(float Value)
{
	OnSFXVolumeChanged.Broadcast(Value);
}

void UPS3ViewModel::RequestVoiceChatEnabledChanged(bool bEnabled)
{
	OnVoiceChatEnabledChanged.Broadcast(bEnabled);
}

void UPS3ViewModel::RequestResolutionChanged(const FString& Resolution)
{
	OnResolutionChanged.Broadcast(Resolution);
}

void UPS3ViewModel::RequestShowTitle()
{
	if (PlayerHUD)
	{
		PlayerHUD->ShowTitle();
	}
}

void UPS3ViewModel::RequestHideTitle()
{
	if (PlayerHUD)
	{
		PlayerHUD->HideTitle();
	}
}

void UPS3ViewModel::RequestGameStart()
{
	OnGameStartRequested.Broadcast();
}

void UPS3ViewModel::RequestGameExit()
{
	OnGameExitRequested.Broadcast();
}

void UPS3ViewModel::RequestShowGameOver()
{
	if (PlayerHUD)
	{
		PlayerHUD->ShowGameOver();
	}
}

void UPS3ViewModel::RequestHideGameOver()
{
	if (PlayerHUD)
	{
		PlayerHUD->HideGameOver();
	}
}

void UPS3ViewModel::RequestGameRestart()
{
	OnGameRestartRequested.Broadcast();
}

void UPS3ViewModel::RequestShowStage5RoleSelect()
{
	if (PlayerHUD)
	{
		PlayerHUD->ShowStage5RoleSelect();
	}
}

void UPS3ViewModel::RequestHideStage5RoleSelect()
{
	if (PlayerHUD)
	{
		PlayerHUD->HideStage5RoleSelect();
	}
}

void UPS3ViewModel::RequestStage5RoleSelection(EPS3PlayerRole SelectedRole)
{
	OnStage5RoleSelectionRequested.Broadcast(SelectedRole);
}

bool UPS3ViewModel::GetIsOpen() const
{
	return bIsOpen;
}

void UPS3ViewModel::SetIsOpen(bool bInIsOpen)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsOpen, bInIsOpen);
}
