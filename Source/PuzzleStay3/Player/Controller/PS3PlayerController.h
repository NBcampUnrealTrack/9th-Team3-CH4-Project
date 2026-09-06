#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "PS3PlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
class UCustomVoiceComponent;
class UVoicePluginControlComponent;
class APS3PlayerState;
class UPS3ViewModel;

UCLASS()
class PUZZLESTAY3_API APS3PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APS3PlayerController();
	virtual void ReceivedPlayer() override;

	// 리스폰 직전에 소유 클라이언트의 입력과 PTT를 정리합니다.
	UFUNCTION(Client, Reliable)
	void Client_PrepareForRespawn();

	// EOS Voice Chat 등 외부 음성 시스템의 초기화 결과를 VoiceComponent에 전달
	UFUNCTION(BlueprintCallable, Category = "PS3|Player Controller|Voice")
	void SetVoiceSystemReady(bool bReady);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnPossess(APawn* InPawn) override;
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS3|Player Controller|UI", meta = (ClampMin = "1"))
	int32 MaxLifeCountForUI = 4;

private:
	UFUNCTION(Client, Reliable)
	void Client_RestoreAfterRespawn();

	void ConfigureLocalInput();
	void RefreshVoiceStateBinding();
	void RefreshLifeStateBinding();
	UPS3ViewModel* GetPS3ViewModel() const;

	UFUNCTION()
	void HandleLifeCountChanged(int32 NewLifeCount);

	UPROPERTY()
	TObjectPtr<APS3PlayerState> BoundLifePlayerState;

	FTimerHandle LifeUIInitializationTimerHandle;

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
