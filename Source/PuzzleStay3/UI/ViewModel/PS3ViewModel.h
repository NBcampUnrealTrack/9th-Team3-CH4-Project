#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "PS3ViewModel.generated.h"

class APlayerHUD;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDoorActivationRequested, int32, DoorIndex, bool, bIsActive);

UCLASS(BlueprintType)
class PUZZLESTAY3_API UPS3ViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	void SetPlayerHUD(APlayerHUD* InPlayerHUD);

	UFUNCTION(BlueprintCallable, Category = "UI|TextNotify")
	void RequestTextNotify(const FText& InDisplayText, float InFontSize, float InDisplayDuration);

	UFUNCTION(BlueprintCallable, Category = "UI|LifeCount")
	void RequestUpdateLifeCount(int32 InCurrentLifeCount, int32 InMaxLifeCount);

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void RequestShowInteractionNotify(FName InNotifyId, const FText& InKeyName);

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void RequestHideInteractionNotify(FName InNotifyId);

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void RequestHideAllInteractionNotifies();

	UFUNCTION(BlueprintCallable, Category = "UI|TimerNotify")
	void RequestTimerNotify(float InDuration);

	UFUNCTION(BlueprintCallable, Category = "UI|TimerNotify")
	void RequestHideTimerNotify();

	UFUNCTION(BlueprintCallable, Category = "UI|TutorialNotify")
	void RequestShowTutorialNotify();

	UFUNCTION(BlueprintCallable, Category = "UI|TutorialNotify")
	void RequestHideTutorialNotify();

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	int32 GetCurrentLifeCount() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetCurrentLifeCount(int32 InCurrentLifeCount);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	int32 GetMaxLifeCount() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetMaxLifeCount(int32 InMaxLifeCount);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	TArray<FText> GetInteractionKeyNames() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetInteractionKeyNames(const TArray<FText>& InInteractionKeyNames);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	bool GetIsInteractionNotifyVisible() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetIsInteractionNotifyVisible(bool bInIsInteractionNotifyVisible);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	bool GetIsDoor1Unlocked() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetIsDoor1Unlocked(bool bInIsDoor1Unlocked);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	bool GetIsDoor2Unlocked() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetIsDoor2Unlocked(bool bInIsDoor2Unlocked);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	bool GetIsDoor3Unlocked() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetIsDoor3Unlocked(bool bInIsDoor3Unlocked);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	bool GetIsDoor4Unlocked() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetIsDoor4Unlocked(bool bInIsDoor4Unlocked);

	UFUNCTION(BlueprintCallable, Category = "UI|DoorOpenButton")
	void RequestDoorActivation(int32 InDoorIndex, bool bIsActive);

	UPROPERTY(BlueprintAssignable, Category = "UI|DoorOpenButton")
	FOnDoorActivationRequested OnDoorActivationRequested;

	UFUNCTION(BlueprintCallable, Category = "UI|VoiceChatIcon")
	void RequestVoiceChatSpeaking(bool bInIsSpeaking);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	bool GetIsOpen() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetIsOpen(bool bInIsOpen);

private:
	UPROPERTY(BlueprintReadOnly, Category = "UI|HUD", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<APlayerHUD> PlayerHUD;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	int32 CurrentLifeCount = 0;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	int32 MaxLifeCount = 0;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	TArray<FText> InteractionKeyNames;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter = "GetIsInteractionNotifyVisible", Setter = "SetIsInteractionNotifyVisible", Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	bool bIsInteractionNotifyVisible = false;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter = "GetIsDoor1Unlocked", Setter = "SetIsDoor1Unlocked", Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	bool bIsDoor1Unlocked = false;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter = "GetIsDoor2Unlocked", Setter = "SetIsDoor2Unlocked", Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	bool bIsDoor2Unlocked = false;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter = "GetIsDoor3Unlocked", Setter = "SetIsDoor3Unlocked", Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	bool bIsDoor3Unlocked = false;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter = "GetIsDoor4Unlocked", Setter = "SetIsDoor4Unlocked", Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	bool bIsDoor4Unlocked = false;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter = "GetIsOpen", Setter = "SetIsOpen", Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	bool bIsOpen = false;
};
