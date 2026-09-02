#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PS3RootHUDWidget.generated.h"

class UTextNotifyWidget;
class ULifeCountWidget;
class UInteractionNotifyWidget;
class UTimerNotifyWidget;
class UTutorialNotifyWidget;
class UDoorOpenButtonWidget;
class UVoiceChatIconWidget;

UCLASS()
class PUZZLESTAY3_API UPS3RootHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UTextNotifyWidget* GetTextNotifyWidget() const;
	ULifeCountWidget* GetLifeCountWidget() const;
	UInteractionNotifyWidget* GetInteractionNotifyWidget() const;
	UTimerNotifyWidget* GetTimerNotifyWidget() const;
	UTutorialNotifyWidget* GetTutorialNotifyWidget() const;
	UDoorOpenButtonWidget* GetDoorOpenButtonWidget() const;
	UVoiceChatIconWidget* GetVoiceChatIconWidget() const;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextNotifyWidget> TextNotifyWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<ULifeCountWidget> LifeCountWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UInteractionNotifyWidget> InteractionNotifyWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTimerNotifyWidget> TimerNotifyWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTutorialNotifyWidget> TutorialNotifyWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UDoorOpenButtonWidget> DoorOpenButtonWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UVoiceChatIconWidget> VoiceChatIconWidget;
};
