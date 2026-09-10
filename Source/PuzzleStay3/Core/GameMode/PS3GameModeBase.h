#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Data/Enum/DoorType.h"

#include "PS3GameModeBase.generated.h"

class APS3PlayerState;
class UInteractionSwitchComponent;
class ADoor;

UENUM(BlueprintType)
enum class EPS3StageNumber : uint8
{
	Stage1,
	Stage3
};

UCLASS()
class PUZZLESTAY3_API APS3GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	void SetPlayerIdentity(APS3PlayerState* NewPlayerState);
	
protected:
	virtual void InitializeToDataAssets();
	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	TObjectPtr<class UBase_GameRuleDataAsset> Base_GameRuleDataAsset;

#pragma region InteractionSwitch

public:
	//b기믹스위치 스위치 개수 저장
	void RegisterInteractionSwitch(UInteractionSwitchComponent* SwitchComp);
	void UnregisterInteractionSwitch(UInteractionSwitchComponent* SwitchComp);

	//모든 b기믹스위치 활성화 여부 확인
	bool AllInteractionSwitchActivated() const;
	
	bool GetInteractionSwitchTimerUsed() const { return bInteractionSwitchTimerUsed; }
	
	void SetInteractionSwitchTimerUsed(bool IsUsed);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameRule|InteractionSwitch")
	TArray<TObjectPtr<UInteractionSwitchComponent>> InteractionSwitches;

private:
	UFUNCTION()
	virtual void HandleSwitchActivatedChanged(bool bActivated);

	FDelegateHandle InteractionSwitchCompoHandle;
	
	bool bInteractionSwitchTimerUsed = false;
	
#pragma endregion

#pragma region OpenDoor

private:
	void OpenEscapeDoor();

#pragma endregion

#pragma region BlokingVolume

public:
	
	virtual void DisableBlockingVolume(EPS3StageNumber StageNumber);

#pragma endregion

#pragma region StageRestart
	//게임오버 시 해당 스테이지 재오픈
protected:
	//스테이지 재시작을 사용하는 스테이지에서 해당 함수 true반환 override
	virtual bool StageRestartIfPlayerDead() const { return false; }

	virtual void StageRestart();
	void ResetAllPlayersDeadState();

private:
	//플레이어 사망 델리게이트 구독함수
	void RegisterPlayerDeadState(APS3PlayerState* PS3PlayerState);

	UFUNCTION()
	void HandlePlayerDeadState(bool bNewIsDead);

	bool bStageRestartRequested = false;
#pragma endregion

#pragma region StageClear

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameRule|StageClear")
	FString NextStageLevelPath;
	
	// 모든 스위치 활성화 후 다음 스테이지로 넘어가기까지 대기 시간.
	float StageClearDelay;
	
	//각 스테이지 클리어 시 사용할 가상함수
	virtual void StageClear();
	void CallStageClearIfTimerOver();

private:
	FTimerHandle StageClearTimerHandle;
	bool bStageClearTimerStarted = false;
#pragma endregion
};
