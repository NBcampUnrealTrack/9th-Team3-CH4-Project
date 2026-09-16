// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PS3PlayerControllerBase.generated.h"

class UPS3ViewModel; // 현준 수정
class UInputAction; // 현준 수정
class UInputMappingContext; // 현준 수정
class USoundClass; // 현준 수정
class USoundMix; // 현준 수정

/**
 * 
 */
UCLASS()
class PUZZLESTAY3_API APS3PlayerControllerBase : public APlayerController
{
	GENERATED_BODY()
	
protected:
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void ReceivedPlayer() override;
	virtual void SetupInputComponent() override; // 현준 수정
	
protected:
	UPROPERTY()
	TObjectPtr<UPS3ViewModel> PS3ViewModel; // 현준 수정

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS3|Player Controller|Input") // 현준 수정
	TObjectPtr<UInputMappingContext> OptionMappingContext; // 현준 수정

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS3|Player Controller|Input") // 현준 수정
	TObjectPtr<UInputAction> OptionAction; // 현준 수정

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Volume") // 현준 수정
	TObjectPtr<USoundMix> MasterSoundMix; // 현준 수정

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Volume") // 현준 수정
	TObjectPtr<USoundClass> MasterSoundClass; // 현준 수정

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Volume") // 현준 수정
	TObjectPtr<USoundClass> SFXSoundClass; // 현준 수정
	
	UFUNCTION()
	void OnClickedRestartGameButton();
	UFUNCTION()
	void OnClickedTitleMenuButton();

	UFUNCTION() // 현준 수정
	void HandleBGMVolumeChanged(float Value); // 현준 수정

	UFUNCTION() // 현준 수정
	void HandleSFXVolumeChanged(float Value); // 현준 수정
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_OnClickedRestartGameButton();
	UFUNCTION(Server, Reliable)
	void ServerRPC_OnClickedTitleButton();
	
public:
	void ConfigureInputMapping();
	
	virtual void ConfigureViewModelBindings(UPS3ViewModel* InViewModel);
	
private:
	void AddOptionMappingContext(); // 현준 수정
	void RemoveOptionMappingContext(); // 현준 수정
	void HandleOptionStarted(); // 현준 수정
	void ApplyOptionPopupInputMode(bool bOpen); // 현준 수정
	void PushConfiguredSoundMix(); // 현준 수정
	void ApplySoundClassVolume(USoundClass* SoundClass, float Value, bool bApplyToChildren); // 현준 수정

	FTimerHandle InitTimerHandle;
	bool bOptionInputConfigured = false; // 현준 수정
	bool bIsOptionPopupOpenByInput = false; // 현준 수정
	
};
