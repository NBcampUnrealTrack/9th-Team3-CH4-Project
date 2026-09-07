#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/Enum/VoiceChatState.h"
#include "VoicePluginControlComponent.generated.h"

class IVoiceChatUser;
struct FVoiceChatResult;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVoiceConversionEnabledChangedSignature, bool, bEnabled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVoiceReadyChangedSignature, bool, bReady);

UCLASS(ClassGroup = (Voice), meta = (BlueprintSpawnableComponent))
class PUZZLESTAY3_API UVoicePluginControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UVoicePluginControlComponent();

	UFUNCTION(BlueprintCallable, Category = "Voice|Plugin")
	void ApplyVoiceChatState(EVoiceChatState NewState);

	UFUNCTION(BlueprintPure, Category = "Voice|Plugin")
	bool IsConversionEnabled() const { return bConversionEnabled; }

	UPROPERTY(BlueprintAssignable, Category = "Voice|Plugin")
	FVoiceConversionEnabledChangedSignature OnConversionEnabledChanged;

	UFUNCTION(BlueprintCallable, Category = "Voice|EOS")
	bool InitializeEOSVoice(int32 LocalUserNum = 0);

	UFUNCTION(BlueprintCallable, Category = "Voice|EOS")
	void ShutdownEOSVoice();

	UFUNCTION(BlueprintCallable, Category = "Voice|EOS")
	void SetTransmitEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Voice|EOS")
	void SetMicrophoneMuted(bool bMuted);

	UFUNCTION(BlueprintPure, Category = "Voice|EOS")
	bool IsVoiceReady() const { return bVoiceReady; }

	UPROPERTY(BlueprintAssignable, Category = "Voice|EOS")
	FVoiceReadyChangedSignature OnVoiceReadyChanged;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void SetConversionEnabled(bool bEnabled);
	void ApplyConversionToPlugin(bool bEnabled);
	void HandleChannelJoined(const FString& ChannelName);
	void HandleChannelExited(const FString& ChannelName, const FVoiceChatResult& Reason);
	void HandleCapturedAudio(TArrayView<int16> PcmSamples, int32 SampleRate, int32 NumChannels);

	IVoiceChatUser* VoiceChatUser = nullptr;

	FDelegateHandle ChannelJoinedHandle;
	FDelegateHandle ChannelExitedHandle;
	FDelegateHandle CapturedAudioHandle;

	bool bVoiceReady = false;
	bool bConversionEnabled = false;
	TAtomic<bool> bConversionEnabledOnAudioThread = false;
};
