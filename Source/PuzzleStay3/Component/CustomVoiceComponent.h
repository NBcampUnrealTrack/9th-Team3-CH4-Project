#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/Enum/VoiceChatState.h"
#include "CustomVoiceComponent.generated.h"

class APS3PlayerState;
class UVoicePluginControlComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnVoiceTransmissionRequestedChanged,
	bool,
	bRequested
);

UCLASS(ClassGroup = (Voice), meta = (BlueprintSpawnableComponent))
class PUZZLESTAY3_API UCustomVoiceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCustomVoiceComponent();

	void BindPlayerState(APS3PlayerState* NewPlayerState);
	bool TryAcquireVoiceObject();
	bool TryReleaseVoiceObject();

	UFUNCTION(BlueprintCallable, Category = "Voice")
	void StartPushToTalk();

	UFUNCTION(BlueprintCallable, Category = "Voice")
	void StopPushToTalk();

	UFUNCTION(BlueprintCallable, Category = "Voice")
	void SetMicrophoneMuted(bool bMuted);

	// 로컬 세션/음성 초기화 담당 코드에서 호출
	UFUNCTION(BlueprintCallable, Category = "Voice")
	void SetVoiceReady(bool bReady);

	UFUNCTION(BlueprintPure, Category = "Voice")
	bool IsMicrophoneMuted() const { return bMicrophoneMuted; }

	UFUNCTION(BlueprintPure, Category = "Voice")
	bool IsTransmissionRequested() const
	{
		return bTransmissionRequested;
	}

	UPROPERTY(BlueprintAssignable, Category = "Voice")
	FOnVoiceTransmissionRequestedChanged
		OnTransmissionRequestedChanged;

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(
		const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void HandleVoiceChatStateChanged(EVoiceChatState NewState);

	void UnbindPlayerState();
	bool SetVoiceObjectHeld(bool bNewIsHeld);
	void UpdateTransmission();

	UPROPERTY()
	TObjectPtr<APS3PlayerState> BoundPlayerState;

	UPROPERTY()
	TObjectPtr<UVoicePluginControlComponent> PluginControlComponent;

	EVoiceChatState VoiceChatState = EVoiceChatState::Default;

	bool bVoiceReady = false;
	bool bPushToTalkHeld = false;
	bool bMicrophoneMuted = false;
	bool bTransmissionRequested = false;
};
