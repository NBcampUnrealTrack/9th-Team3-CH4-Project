#include "CustomVoiceComponent.h"

#include "Core/GameMode/PS3GameModeS3.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Player/PlayerState/PS3PlayerState.h"
#include "VoicePluginControlComponent.h"


UCustomVoiceComponent::UCustomVoiceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCustomVoiceComponent::BeginPlay()
{
	Super::BeginPlay();

	PluginControlComponent =
		GetOwner()->FindComponentByClass<
			UVoicePluginControlComponent
		>();

	if (APlayerController* PC =
		Cast<APlayerController>(GetOwner()))
	{
		BindPlayerState(
			PC->GetPlayerState<APS3PlayerState>()
		);
	}

	RegisterToStage3GameMode();
}

void UCustomVoiceComponent::RegisterToStage3GameMode()
{
	// GameMode는 서버에만 존재합니다.
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	if (APS3GameModeS3* GameMode =
		GetWorld()->GetAuthGameMode<APS3GameModeS3>())
	{
		GameMode->RegisterCustomVoiceComponent(this);
	}
}

void UCustomVoiceComponent::BindPlayerState(
	APS3PlayerState* NewPlayerState
)
{
	if (BoundPlayerState == NewPlayerState)
	{
		return;
	}

	UnbindPlayerState();

	BoundPlayerState = NewPlayerState;

	if (!IsValid(BoundPlayerState))
	{
		HandleVoiceChatStateChanged(EVoiceChatState::Inactive);
		return;
	}

	BoundPlayerState->OnVoiceChatStateChanged.AddUniqueDynamic(
		this,
		&ThisClass::HandleVoiceChatStateChanged
	);

	// 바인딩 전에 이미 복제된 상태도 즉시 반영
	HandleVoiceChatStateChanged(
		BoundPlayerState->GetVoiceChatState()
	);
}

void UCustomVoiceComponent::UnbindPlayerState()
{
	if (IsValid(BoundPlayerState))
	{
		BoundPlayerState->OnVoiceChatStateChanged.RemoveDynamic(
			this,
			&ThisClass::HandleVoiceChatStateChanged
		);
	}

	BoundPlayerState = nullptr;
}

bool UCustomVoiceComponent::TryAcquireVoiceObject()
{
	return SetVoiceObjectHeld(true);
}

bool UCustomVoiceComponent::TryReleaseVoiceObject()
{
	return SetVoiceObjectHeld(false);
}

bool UCustomVoiceComponent::SetVoiceObjectHeld(
	const bool bNewIsHeld
)
{
	if (!GetOwner()->HasAuthority() ||
		!IsValid(BoundPlayerState))
	{
		return false;
	}

	if (BoundPlayerState->IsVoiceObjectHeld() ==
		bNewIsHeld)
	{
		return false;
	}

	BoundPlayerState->SetVoiceObjectHeld(bNewIsHeld);

	// GameModeS3가 전체 보유 인원을 다시 계산합니다.
	OnVoiceObjectHeld.Broadcast();

	return true;
}

void UCustomVoiceComponent::HandleVoiceChatStateChanged(
	const EVoiceChatState NewState
)
{
	VoiceChatState = NewState;

	if (!IsValid(PluginControlComponent))
	{
		PluginControlComponent =
			GetOwner()->FindComponentByClass<
				UVoicePluginControlComponent
			>();
	}

	if (IsValid(PluginControlComponent))
	{
		PluginControlComponent->ApplyVoiceChatState(
			VoiceChatState
		);
	}

	UpdateTransmission();
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
	
	const bool bGameAllowsTransmission = VoiceChatState != EVoiceChatState::Inactive;
	const bool bShouldTransmit =
		bVoiceReady &&
		bPushToTalkHeld &&
		!bMicrophoneMuted &&
		bGameAllowsTransmission;

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
	OnVoiceObjectHeld.Clear();
	UnbindPlayerState();
	SetVoiceReady(false);

	Super::EndPlay(EndPlayReason);
}
