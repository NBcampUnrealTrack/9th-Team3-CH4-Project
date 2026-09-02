#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VoiceChatIconWidget.generated.h"

UCLASS()
class PUZZLESTAY3_API UVoiceChatIconWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|VoiceChatIcon")
	void ShowVoiceChatIcon();

	UFUNCTION(BlueprintCallable, Category = "UI|VoiceChatIcon")
	void HideVoiceChatIcon();

	UFUNCTION(BlueprintCallable, Category = "UI|VoiceChatIcon")
	void SetSpeaking(bool bInIsSpeaking);
};
