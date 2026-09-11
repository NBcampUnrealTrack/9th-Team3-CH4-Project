#include "VoicePluginControlComponent.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "IOnlineSubsystemEOS.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemNames.h"
#include "OnlineSubsystemUtils.h"
#include "TimerManager.h"
#include "VoiceChat.h"

UVoicePluginControlComponent::UVoicePluginControlComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UVoicePluginControlComponent::ApplyVoiceChatState(const EVoiceChatState NewState)
{
	SetConversionEnabled(NewState == EVoiceChatState::Conversion);
}

bool UVoicePluginControlComponent::InitializeEOSVoice(const int32 LocalUserNum)
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!IsValid(PC) || !PC->IsLocalController())
	{
		return false;
	}

	// 로비와 동일한 월드의 EOS 사용자를 제어합니다 (PIE 인스턴스 분리).
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld(), EOS_SUBSYSTEM);
	if (OnlineSubsystem == nullptr)
	{
		return false;
	}

	const IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
	if (!Identity.IsValid())
	{
		return false;
	}

	const TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(LocalUserNum);
	if (!UserId.IsValid())
	{
		return false;
	}

	IOnlineSubsystemEOS* EOSSubsystem = static_cast<IOnlineSubsystemEOS*>(OnlineSubsystem);
	IVoiceChatUser* NewVoiceChatUser = EOSSubsystem->GetVoiceChatUserInterface(*UserId);
	if (NewVoiceChatUser == nullptr)
	{
		return false;
	}
	if (VoiceChatUser == NewVoiceChatUser)
	{
		// BP에서 초기화를 다시 호출해도 콜백을 중복 등록하지 않습니다.
		OnVoiceReadyChanged.Broadcast(bVoiceReady);
		return true;
	}
	if (VoiceChatUser != nullptr)
	{
		ShutdownEOSVoice();
	}

	VoiceChatUser = NewVoiceChatUser;

	ChannelJoinedHandle = VoiceChatUser->OnVoiceChatChannelJoined().AddUObject(this, &ThisClass::HandleChannelJoined);
	ChannelExitedHandle = VoiceChatUser->OnVoiceChatChannelExited().AddUObject(this, &ThisClass::HandleChannelExited);
	PlayerAddedHandle = VoiceChatUser->OnVoiceChatPlayerAdded().AddUObject(this, &ThisClass::HandlePlayerAdded);
	CapturedAudioHandle = VoiceChatUser->RegisterOnVoiceChatAfterCaptureAudioReadDelegate(
		FOnVoiceChatAfterCaptureAudioReadDelegate::FDelegate::CreateUObject(this, &ThisClass::HandleCapturedAudio));

	bVoiceReady = !VoiceChatUser->GetChannels().IsEmpty();
	VoiceChatUser->TransmitToNoChannels();
	OnVoiceReadyChanged.Broadcast(bVoiceReady);

	return true;
}

void UVoicePluginControlComponent::ShutdownEOSVoice()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PlayerReceivingRetryTimerHandle);
	}
	PendingVoiceChannelName.Reset();
	PendingVoicePlayerName.Reset();

	if (VoiceChatUser == nullptr)
	{
		return;
	}

	VoiceChatUser->TransmitToNoChannels();

	if (ChannelJoinedHandle.IsValid())
	{
		VoiceChatUser->OnVoiceChatChannelJoined().Remove(ChannelJoinedHandle);
		ChannelJoinedHandle.Reset();
	}

	if (ChannelExitedHandle.IsValid())
	{
		VoiceChatUser->OnVoiceChatChannelExited().Remove(ChannelExitedHandle);
		ChannelExitedHandle.Reset();
	}

	if (PlayerAddedHandle.IsValid())
	{
		VoiceChatUser->OnVoiceChatPlayerAdded().Remove(PlayerAddedHandle);
		PlayerAddedHandle.Reset();
	}

	if (CapturedAudioHandle.IsValid())
	{
		VoiceChatUser->UnregisterOnVoiceChatAfterCaptureAudioReadDelegate(CapturedAudioHandle);
		CapturedAudioHandle.Reset();
	}

	VoiceChatUser = nullptr;
	bVoiceReady = false;
	OnVoiceReadyChanged.Broadcast(false);
}

void UVoicePluginControlComponent::SetTransmitEnabled(const bool bEnabled)
{
	if (VoiceChatUser == nullptr)
	{
		return;
	}

	if (bEnabled && bVoiceReady)
	{
		VoiceChatUser->TransmitToAllChannels();
	}
	else
	{
		VoiceChatUser->TransmitToNoChannels();
	}
}

void UVoicePluginControlComponent::SetMicrophoneMuted(const bool bMuted)
{
	if (VoiceChatUser != nullptr)
	{
		VoiceChatUser->SetAudioInputDeviceMuted(bMuted);
	}
}

void UVoicePluginControlComponent::SetConversionEnabled(const bool bEnabled)
{
	if (bConversionEnabled == bEnabled)
	{
		return;
	}

	bConversionEnabled = bEnabled;
	bConversionEnabledOnAudioThread.Store(bEnabled);
	ApplyConversionToPlugin(bEnabled);
	OnConversionEnabledChanged.Broadcast(bEnabled);
}

void UVoicePluginControlComponent::ApplyConversionToPlugin(const bool bEnabled)
{
	UE_LOG(LogTemp, Log, TEXT("Voice conversion: %s"), bEnabled ? TEXT("Enabled") : TEXT("Disabled"));
}

void UVoicePluginControlComponent::HandleChannelJoined(const FString& ChannelName)
{
	if (VoiceChatUser == nullptr)
	{
		return;
	}

	bVoiceReady = true;
	VoiceChatUser->TransmitToNoChannels();
	OnVoiceReadyChanged.Broadcast(true);
	UE_LOG(LogTemp, Log, TEXT("EOS voice channel joined: %s"), *ChannelName);
}

void UVoicePluginControlComponent::HandleChannelExited(
	const FString& ChannelName,
	const FVoiceChatResult& Reason)
{
	bVoiceReady = VoiceChatUser != nullptr && !VoiceChatUser->GetChannels().IsEmpty();
	OnVoiceReadyChanged.Broadcast(bVoiceReady);
	UE_LOG(
		LogTemp,
		Log,
		TEXT("EOS voice channel exited: %s, result: %s, detail: %s"),
		*ChannelName,
		*LexToString(Reason.ResultCode),
		*Reason.ErrorDesc);
}

void UVoicePluginControlComponent::HandlePlayerAdded(
	const FString& ChannelName,
	const FString& PlayerName)
{
	if (VoiceChatUser == nullptr || ChannelName.IsEmpty() || PlayerName.IsEmpty())
	{
		return;
	}

	PendingVoiceChannelName = ChannelName;
	PendingVoicePlayerName = PlayerName;
	ReapplyPlayerReceiving();

	// EOS P2P가 참가자의 차단 상태를 비동기로 갱신한 뒤 수신 옵션을 한 번 더 적용합니다.
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PlayerReceivingRetryTimerHandle);
		World->GetTimerManager().SetTimer(
			PlayerReceivingRetryTimerHandle,
			this,
			&ThisClass::ReapplyPlayerReceiving,
			0.25f,
			false);
	}
}

void UVoicePluginControlComponent::ReapplyPlayerReceiving()
{
	if (VoiceChatUser == nullptr || PendingVoiceChannelName.IsEmpty() || PendingVoicePlayerName.IsEmpty())
	{
		return;
	}

	VoiceChatUser->SetChannelPlayerMuted(PendingVoiceChannelName, PendingVoicePlayerName, false);

	// 이미 음소거 해제 상태면 위 호출이 no-op이므로 볼륨을 다시 설정해 EOS 수신 옵션을 강제로 갱신합니다.
	const float CurrentVolume = VoiceChatUser->GetPlayerVolume(PendingVoicePlayerName);
	VoiceChatUser->SetPlayerVolume(PendingVoicePlayerName, CurrentVolume);

	UE_LOG(LogTemp, Log, TEXT("EOS voice receiving reapplied: channel=%s, player=%s"),
		*PendingVoiceChannelName, *PendingVoicePlayerName);
}

void UVoicePluginControlComponent::HandleCapturedAudio(
	TArrayView<int16> PcmSamples,
	const int32 SampleRate,
	const int32 NumChannels)
{
	if (!bConversionEnabledOnAudioThread.Load())
	{
		return;
	}

	constexpr int32 QuantizationStep = 2048;
	for (int16& Sample : PcmSamples)
	{
		const int32 Quantized = (static_cast<int32>(Sample) / QuantizationStep) * QuantizationStep;
		Sample = static_cast<int16>(FMath::Clamp(Quantized, -32768, 32767));
	}
}

void UVoicePluginControlComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ShutdownEOSVoice();
	Super::EndPlay(EndPlayReason);
}
