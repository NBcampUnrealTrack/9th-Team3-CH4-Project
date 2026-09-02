#include "VoiceChatIconWidget.h"

void UVoiceChatIconWidget::SetSpeaking(bool bInIsSpeaking)
{
	SetVisibility(bInIsSpeaking ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}
