#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "PS3PlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
class UCustomVoiceComponent;
class UVoicePluginControlComponent;

UCLASS()
class PUZZLESTAY3_API APS3PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APS3PlayerController();
	virtual void ReceivedPlayer() override;

	// EOS Voice Chat 등 외부 음성 시스템의 초기화 결과를 VoiceComponent에 전달
	UFUNCTION(BlueprintCallable, Category = "PS3|Player Controller|Voice")
	void SetVoiceSystemReady(bool bReady);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnRep_PlayerState() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS3|Player Controller|Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS3|Player Controller|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS3|Player Controller|Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS3|Player Controller|Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS3|Player Controller|Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS3|Player Controller|Input")
	TObjectPtr<UInputAction> DropAction;
	
	//말하기 입력 액션
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
		Category = "PS3|Player Controller|Input")
	TObjectPtr<UInputAction> PushToTalkAction;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Voice",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCustomVoiceComponent> VoiceComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Voice",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UVoicePluginControlComponent> VoicePluginControlComponent;

private:
	void ConfigureLocalInput();
	void RefreshVoiceStateBinding();

	bool bLocalInputConfigured = false;

	void HandleMoveInput(const FInputActionValue& InValue);
	void HandleLookInput(const FInputActionValue& InValue);
	void HandleJumpStarted();
	void HandleJumpCompleted();
	void HandleInteractStarted();
	void HandleDropStarted();
	void HandleVoiceStarted();
	void HandleVoiceStopped();
	
};
