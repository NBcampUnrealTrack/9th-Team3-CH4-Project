#include "PS3ViewModel.h"

#include "../HUD/PlayerHUD.h"

void UPS3ViewModel::SetPlayerHUD(APlayerHUD* InPlayerHUD)
{
	PlayerHUD = InPlayerHUD;
}

void UPS3ViewModel::RequestTextNotify(const FText& InDisplayText, float InFontSize, float InDisplayDuration)
{
	UE_LOG(
		LogTemp,
		Log,
		TEXT("RootHUDTest PS3ViewModel::RequestTextNotify Entry Text=%s"),
		*InDisplayText.ToString()
	);

	if (PlayerHUD)
	{
		PlayerHUD->ShowTextNotify(InDisplayText, InFontSize, InDisplayDuration);
	}
}

void UPS3ViewModel::RequestUpdateLifeCount(int32 InCurrentLifeCount, int32 InMaxLifeCount)
{
	if (PlayerHUD)
	{
		PlayerHUD->UpdateLifeCount(InCurrentLifeCount, InMaxLifeCount);
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

void UPS3ViewModel::RequestTimerNotify(float InRemainingTime, float InTotalTime)
{
	if (PlayerHUD)
	{
		PlayerHUD->UpdateTimerNotify(InRemainingTime, InTotalTime);
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

float UPS3ViewModel::GetTimerNotifyProgress() const
{
	return TimerNotifyProgress;
}

void UPS3ViewModel::SetTimerNotifyProgress(float InTimerNotifyProgress)
{
	UE_MVVM_SET_PROPERTY_VALUE(TimerNotifyProgress, InTimerNotifyProgress);
}

bool UPS3ViewModel::GetIsTimerNotifyVisible() const
{
	return bIsTimerNotifyVisible;
}

void UPS3ViewModel::SetIsTimerNotifyVisible(bool bInIsTimerNotifyVisible)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsTimerNotifyVisible, bInIsTimerNotifyVisible);
}

bool UPS3ViewModel::GetIsTutorialNotifyVisible() const
{
	return bIsTutorialNotifyVisible;
}

void UPS3ViewModel::SetIsTutorialNotifyVisible(bool bInIsTutorialNotifyVisible)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsTutorialNotifyVisible, bInIsTutorialNotifyVisible);
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

void UPS3ViewModel::RequestOpenDoor(int32 InDoorIndex)
{
	if (InDoorIndex < 1 || InDoorIndex > 4)
	{
		return;
	}

	OnDoorOpenRequested.Broadcast(InDoorIndex);
}

bool UPS3ViewModel::GetIsSpeaking() const
{
	return bIsSpeaking;
}

void UPS3ViewModel::SetIsSpeaking(bool bInIsSpeaking)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsSpeaking, bInIsSpeaking);
}

bool UPS3ViewModel::GetIsOpen() const
{
	return bIsOpen;
}

void UPS3ViewModel::SetIsOpen(bool bInIsOpen)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsOpen, bInIsOpen);
}
