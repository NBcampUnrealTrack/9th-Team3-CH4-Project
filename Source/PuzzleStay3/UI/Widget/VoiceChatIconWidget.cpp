#include "VoiceChatIconWidget.h"

void UVoiceChatIconWidget::ShowVoiceChatIcon()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UVoiceChatIconWidget::HideVoiceChatIcon()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UVoiceChatIconWidget::SetSpeaking(bool bInIsSpeaking)
{
	if (bInIsSpeaking)
	{
		ShowVoiceChatIcon();
	}
	else
	{
		HideVoiceChatIcon();
	}
}
