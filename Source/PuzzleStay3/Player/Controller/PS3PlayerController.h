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
class AJeoul;

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

	// 서버에서 대상 Controller에 호출. GameMode가 결정한 TrapId별 결과를 전달합니다.
	UFUNCTION(Client, Reliable)
	void Client_ReceiveStage3Visibility(const TArray<int32>& TrapIds, const TArray<bool>& Results);

	// 서버가 시점을 전환할 플레이어의 Controller에 호출합니다.
	UFUNCTION(Client, Reliable, BlueprintCallable, Category = "PS3|Cutscene")
	void Client_BeginJeoulCutscene(AJeoul* Jeoul);

	UFUNCTION(BlueprintCallable, Category = "PS3|Cutscene")
	void EndJeoulCutscene();

	UFUNCTION(BlueprintPure, Category = "PS3|Cutscene")
	bool IsJeoulCutsceneActive() const { return bJeoulCutsceneActive; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "PS3|Cutscene", meta = (ClampMin = "0.0"))
	float JeoulCameraBlendTime = 0.35f;

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
	
	UFUNCTION(BlueprintCallable, Category = "PS3|Player Controller|Voice")
	bool InitializeVoiceSystem(int32 LocalUserNum = 0);

	UFUNCTION(BlueprintCallable, Category = "PS3|Player Controller|Voice")
	void ShutdownVoiceSystem();

private:
	void HandleJeoulCheckFinished(bool bIsSuccess);
	UFUNCTION()
	void HandleCutsceneTargetDestroyed(AActor* DestroyedActor);
	TWeakObjectPtr<AJeoul> ActiveCutsceneJeoul;
	TWeakObjectPtr<AActor> PreviousCutsceneViewTarget;
	FDelegateHandle JeoulCheckFinishedHandle;
	bool bJeoulCutsceneActive = false;

	void ApplyStage3Visibility();
	TMap<int32, bool> Stage3VisibilityByTrapId;
	FTimerHandle Stage3VisibilityTimerHandle;

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
