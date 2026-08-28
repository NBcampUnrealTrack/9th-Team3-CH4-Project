#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VoiceChatIconWidget.generated.h"

UCLASS()
class PUZZLESTAY3_API UVoiceChatIconWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|VoiceChatIcon")
	bool bIsSpeaking = false;
};
