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
class UOptionPopupWidget;
class UTitleWidget;
class UGameOverWidget;
class UStage5RoleSelectWidget;

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
	UOptionPopupWidget* GetOptionPopupWidget() const;
	UTitleWidget* GetTitleWidget() const;
	UGameOverWidget* GetGameOverWidget() const;
	UStage5RoleSelectWidget* GetStage5RoleSelectWidget() const;

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

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UOptionPopupWidget> OptionPopupWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTitleWidget> TitleWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UGameOverWidget> GameOverWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UStage5RoleSelectWidget> Stage5RoleSelectWidget;
};
