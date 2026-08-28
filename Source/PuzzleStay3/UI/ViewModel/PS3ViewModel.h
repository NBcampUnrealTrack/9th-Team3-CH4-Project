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
	float GetDisplayDuration() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetDisplayDuration(float InDisplayDuration);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	int32 GetCurrentRemainingLife() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetCurrentRemainingLife(int32 InCurrentRemainingLife);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	FText GetKeyName() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetKeyName(const FText& InKeyName);

	UFUNCTION(BlueprintPure, FieldNotify, Category = "UI|ViewModel")
	FText GetActionName() const;

	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetActionName(const FText& InActionName);

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
	float DisplayDuration = 0.0f;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	int32 CurrentRemainingLife = 0;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	FText KeyName;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	FText ActionName;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	int32 TokenID = 0;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter = "GetIsUnlocked", Setter = "SetIsUnlocked", Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	bool bIsUnlocked = false;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter = "GetIsSpeaking", Setter = "SetIsSpeaking", Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	bool bIsSpeaking = false;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter = "GetIsOpen", Setter = "SetIsOpen", Category = "UI|ViewModel", meta = (AllowPrivateAccess = "true"))
	bool bIsOpen = false;
};
