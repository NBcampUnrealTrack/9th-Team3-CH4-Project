#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/Enum/VoiceChatState.h"
#include "VoicePluginControlComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FVoiceConversionEnabledChangedSignature,
	bool,
	bEnabled
);

UCLASS(ClassGroup = (Voice), meta = (BlueprintSpawnableComponent))
class PUZZLESTAY3_API UVoicePluginControlComponent
	: public UActorComponent
{
	GENERATED_BODY()

public:
	UVoicePluginControlComponent();

	UFUNCTION(BlueprintCallable, Category = "Voice|Plugin")
	void ApplyVoiceChatState(EVoiceChatState NewState);

	UFUNCTION(BlueprintPure, Category = "Voice|Plugin")
	bool IsConversionEnabled() const
	{
		return bConversionEnabled;
	}

	UPROPERTY(BlueprintAssignable, Category = "Voice|Plugin")
	FVoiceConversionEnabledChangedSignature
		OnConversionEnabledChanged;

private:
	void SetConversionEnabled(bool bEnabled);
	void ApplyConversionToPlugin(bool bEnabled);

	bool bConversionEnabled = false;
};