#include "PS3ViewModel.h"

#include "../HUD/PlayerHUD.h"

void UPS3ViewModel::SetPlayerHUD(APlayerHUD* InPlayerHUD)
{
	PlayerHUD = InPlayerHUD;
}

void UPS3ViewModel::RequestTextNotify(EPS3TextNotifyType NotifyType)
{
	if (PlayerHUD)
	{
		PlayerHUD->ShowTextNotify(NotifyType);
	}
}

void UPS3ViewModel::RequestSetTextNotifyVisible(bool bVisible)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetTextNotifyVisible(bVisible);
	}
}

void UPS3ViewModel::RequestSetLifeCountVisible(bool bVisible)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetLifeCountVisible(bVisible);
	}
}

void UPS3ViewModel::RequestUpdateLifeCount(int32 InCurrentLifeCount, int32 InMaxLifeCount)
{
	if (PlayerHUD)
	{
		PlayerHUD->UpdateLifeCount(InCurrentLifeCount, InMaxLifeCount);
	}
}

void UPS3ViewModel::RequestSetInteractionNotifyVisible(bool bVisible)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetInteractionNotifyVisible(bVisible);
	}
}

void UPS3ViewModel::RequestShowInteractionNotify(EPS3InteractionNotifyType NotifyType)
{
	if (PlayerHUD)
	{
		PlayerHUD->ShowInteractionNotify(NotifyType);
	}
}

void UPS3ViewModel::RequestHideInteractionNotify(EPS3InteractionNotifyType NotifyType)
{
	if (PlayerHUD)
	{
		PlayerHUD->HideInteractionNotify(NotifyType);
	}
}

void UPS3ViewModel::RequestHideAllInteractionNotifies()
{
	if (PlayerHUD)
	{
		PlayerHUD->HideAllInteractionNotifies();
	}
}

void UPS3ViewModel::RequestSetTimerNotifyVisible(bool bVisible)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetTimerNotifyVisible(bVisible);
	}
}

void UPS3ViewModel::RequestTimerNotify(FName InTimerId, float InDuration)
{
	if (PlayerHUD)
	{
		PlayerHUD->UpdateTimerNotify(InTimerId, InDuration);
	}
}

void UPS3ViewModel::RequestReduceTimerNotify(FName InTimerId, float InReduceTime)
{
	if (PlayerHUD)
	{
		PlayerHUD->ReduceTimerNotify(InTimerId, InReduceTime);
	}
}

void UPS3ViewModel::RequestResetTimerNotify()
{
	if (PlayerHUD)
	{
		PlayerHUD->ResetTimerNotify();
	}
}

void UPS3ViewModel::RequestSetTutorialNotifyVisible(bool bVisible)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetTutorialNotifyVisible(bVisible);
	}
}

void UPS3ViewModel::RequestSetDoorOpenButtonVisible(bool bVisible)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetDoorOpenButtonVisible(bVisible);
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

	OnDoorActivationRequested_UI.Broadcast(InDoorIndex, bIsActive);
}

void UPS3ViewModel::RequestVoiceChatSpeaking(bool bInIsSpeaking)
{
	if (PlayerHUD)
	{
		PlayerHUD->UpdateVoiceChatIcon(bInIsSpeaking);
	}
}

void UPS3ViewModel::RequestSetVoiceChatIconVisible(bool bVisible)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetVoiceChatIconVisible(bVisible);
	}
}

void UPS3ViewModel::RequestSetOptionPopupVisible(bool bVisible)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetOptionPopupVisible(bVisible);
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
	OnExitToMainRequested_UI.Broadcast();
}

void UPS3ViewModel::RequestBGMVolumeChanged(float Value)
{
	OnBGMVolumeChanged_UI.Broadcast(Value);
}

void UPS3ViewModel::RequestSFXVolumeChanged(float Value)
{
	OnSFXVolumeChanged_UI.Broadcast(Value);
}

void UPS3ViewModel::RequestVoiceChatEnabledChanged(bool bEnabled)
{
	OnVoiceChatEnabledChanged_UI.Broadcast(bEnabled);
}

void UPS3ViewModel::RequestResolutionChanged(const FString& Resolution)
{
	OnResolutionChanged_UI.Broadcast(Resolution);
}

void UPS3ViewModel::RequestSetTitleVisible(bool bVisible)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetTitleVisible(bVisible);
	}
}

void UPS3ViewModel::RequestGameStart()
{
	OnGameStartRequested_UI.Broadcast();
}

void UPS3ViewModel::RequestGameExit()
{
	OnGameExitRequested_UI.Broadcast();
}

void UPS3ViewModel::RequestSetGameOverVisible(bool bVisible)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetGameOverVisible(bVisible);
	}
}

void UPS3ViewModel::RequestGameRestart()
{
	OnGameRestartRequested_UI.Broadcast();
}

void UPS3ViewModel::RequestSetStage5RoleSelectVisible(bool bVisible)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetStage5RoleSelectVisible(bVisible);
	}
}

void UPS3ViewModel::RequestStage5RoleSelection(EPS3PlayerRole SelectedRole)
{
	OnStage5RoleSelectionRequested_UI.Broadcast(SelectedRole);
}

bool UPS3ViewModel::GetIsOpen() const
{
	return bIsOpen;
}

void UPS3ViewModel::SetIsOpen(bool bInIsOpen)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsOpen, bInIsOpen);
}
