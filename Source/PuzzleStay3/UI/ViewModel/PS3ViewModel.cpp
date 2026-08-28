#include "PS3ViewModel.h"

FText UPS3ViewModel::GetDisplayText() const
{
	return DisplayText;
}

void UPS3ViewModel::SetDisplayText(const FText& InDisplayText)
{
	UE_MVVM_SET_PROPERTY_VALUE(DisplayText, InDisplayText);
}

float UPS3ViewModel::GetDisplayDuration() const
{
	return DisplayDuration;
}

void UPS3ViewModel::SetDisplayDuration(float InDisplayDuration)
{
	UE_MVVM_SET_PROPERTY_VALUE(DisplayDuration, InDisplayDuration);
}

int32 UPS3ViewModel::GetCurrentRemainingLife() const
{
	return CurrentRemainingLife;
}

void UPS3ViewModel::SetCurrentRemainingLife(int32 InCurrentRemainingLife)
{
	UE_MVVM_SET_PROPERTY_VALUE(CurrentRemainingLife, InCurrentRemainingLife);
}

FText UPS3ViewModel::GetKeyName() const
{
	return KeyName;
}

void UPS3ViewModel::SetKeyName(const FText& InKeyName)
{
	UE_MVVM_SET_PROPERTY_VALUE(KeyName, InKeyName);
}

FText UPS3ViewModel::GetActionName() const
{
	return ActionName;
}

void UPS3ViewModel::SetActionName(const FText& InActionName)
{
	UE_MVVM_SET_PROPERTY_VALUE(ActionName, InActionName);
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
