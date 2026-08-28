#include "PS3ViewModel.h"

FText UPS3ViewModel::GetDisplayText() const
{
	return DisplayText;
}

void UPS3ViewModel::SetDisplayText(const FText& InDisplayText)
{
	UE_MVVM_SET_PROPERTY_VALUE(DisplayText, InDisplayText);
}

float UPS3ViewModel::GetFontSize() const
{
	return FontSize;
}

void UPS3ViewModel::SetFontSize(float InFontSize)
{
	UE_MVVM_SET_PROPERTY_VALUE(FontSize, InFontSize);
}

bool UPS3ViewModel::GetIsTextNotifyVisible() const
{
	return bIsTextNotifyVisible;
}

void UPS3ViewModel::SetIsTextNotifyVisible(bool bInIsTextNotifyVisible)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsTextNotifyVisible, bInIsTextNotifyVisible);
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

int32 UPS3ViewModel::GetTokenID() const
{
	return TokenID;
}

void UPS3ViewModel::SetTokenID(int32 InTokenID)
{
	UE_MVVM_SET_PROPERTY_VALUE(TokenID, InTokenID);
}

bool UPS3ViewModel::GetIsUnlocked() const
{
	return bIsUnlocked;
}

void UPS3ViewModel::SetIsUnlocked(bool bInIsUnlocked)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsUnlocked, bInIsUnlocked);
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
