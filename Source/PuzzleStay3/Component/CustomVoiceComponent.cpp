#include "CustomVoiceComponent.h"

#include "GameFramework/PlayerController.h"

UCustomVoiceComponent::UCustomVoiceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCustomVoiceComponent::StartPushToTalk()
{
	// 준비 전에 누른 입력으로 나중에 갑자기 송신하지 않도록 처리
	if (!bVoiceReady)
	{
		return;
	}

	bPushToTalkHeld = true;
	UpdateTransmission();
}

void UCustomVoiceComponent::StopPushToTalk()
{
	bPushToTalkHeld = false;
	UpdateTransmission();
}

void UCustomVoiceComponent::SetMicrophoneMuted(bool bMuted)
{
	bMicrophoneMuted = bMuted;
	UpdateTransmission();
}

void UCustomVoiceComponent::SetVoiceReady(bool bReady)
{
	bVoiceReady = bReady;

	if (!bReady)
	{
		bPushToTalkHeld = false;
	}

	UpdateTransmission();
}

void UCustomVoiceComponent::UpdateTransmission()
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());

	if (!IsValid(PC) || !PC->IsLocalController())
	{
		return;
	}

	const bool bShouldTransmit = bVoiceReady && bPushToTalkHeld && !bMicrophoneMuted;

	if (bTransmissionRequested == bShouldTransmit)
	{
		return;
	}

	PC->ToggleSpeaking(bShouldTransmit);

	bTransmissionRequested = bShouldTransmit;
	OnTransmissionRequestedChanged.Broadcast(bTransmissionRequested);
}

void UCustomVoiceComponent::EndPlay(
	const EEndPlayReason::Type EndPlayReason)
{
	SetVoiceReady(false);

	Super::EndPlay(EndPlayReason);
}