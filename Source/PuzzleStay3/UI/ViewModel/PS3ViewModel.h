#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "PS3ViewModel.generated.h"

UCLASS(BlueprintType)
class PUZZLESTAY3_API UPS3ViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	FText GetDisplayText() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetDisplayText(const FText& InDisplayText);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	float GetFontSize() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetFontSize(float InFontSize);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	bool GetIsTextNotifyVisible() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetIsTextNotifyVisible(bool bInIsTextNotifyVisible);

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
	float GetTimerNotifyProgress() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetTimerNotifyProgress(float InTimerNotifyProgress);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	bool GetIsTimerNotifyVisible() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetIsTimerNotifyVisible(bool bInIsTimerNotifyVisible);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	int32 GetTokenID() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetTokenID(int32 InTokenID);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	bool GetIsUnlocked() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetIsUnlocked(bool bInIsUnlocked);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	bool GetIsSpeaking() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetIsSpeaking(bool bInIsSpeaking);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	bool GetIsOpen() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetIsOpen(bool bInIsOpen);

private:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	FText DisplayText;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	float FontSize = 0.0f;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter = "GetIsTextNotifyVisible", Setter = "SetIsTextNotifyVisible", Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	bool bIsTextNotifyVisible = false;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	int32 CurrentLifeCount = 0;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	int32 MaxLifeCount = 0;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	TArray<FText> InteractionKeyNames;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter = "GetIsInteractionNotifyVisible", Setter = "SetIsInteractionNotifyVisible", Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	bool bIsInteractionNotifyVisible = false;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	float TimerNotifyProgress = 0.0f;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter = "GetIsTimerNotifyVisible", Setter = "SetIsTimerNotifyVisible", Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	bool bIsTimerNotifyVisible = false;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	int32 TokenID = 0;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter = "GetIsUnlocked", Setter = "SetIsUnlocked", Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	bool bIsUnlocked = false;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter = "GetIsSpeaking", Setter = "SetIsSpeaking", Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	bool bIsSpeaking = false;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter = "GetIsOpen", Setter = "SetIsOpen", Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	bool bIsOpen = false;
};
