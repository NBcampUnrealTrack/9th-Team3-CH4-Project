#include "VoicePluginControlComponent.h"

UVoicePluginControlComponent::UVoicePluginControlComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UVoicePluginControlComponent::ApplyVoiceChatState(
	const EVoiceChatState NewState
)
{
	SetConversionEnabled(
		NewState == EVoiceChatState::Conversion
	);
}

void UVoicePluginControlComponent::SetConversionEnabled(
	const bool bEnabled
)
{
	if (bConversionEnabled == bEnabled)
	{
		return;
	}

	bConversionEnabled = bEnabled;

	ApplyConversionToPlugin(bConversionEnabled);
	OnConversionEnabledChanged.Broadcast(bConversionEnabled);
}

void UVoicePluginControlComponent::ApplyConversionToPlugin(
	const bool bEnabled
)
{
	// 실제 Voice Plugin API가 정해지면 이곳에 연결합니다.
	//
	// 예시:
	// VoicePlugin->SetPitchEnabled(bEnabled);
	// VoicePlugin->SetPitchScale(bEnabled ? 0.7f : 1.0f);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Voice conversion: %s"),
		bEnabled ? TEXT("Enabled") : TEXT("Disabled")
	);
}