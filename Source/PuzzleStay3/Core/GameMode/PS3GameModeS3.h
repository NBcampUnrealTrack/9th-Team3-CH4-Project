
#pragma once

#include "CoreMinimal.h"
#include "PS3GameModeBase.h"
#include "Data/Enum/VoiceChatState.h"
#include "Data/DataAsset/S3_GameRuleDataAsset.h"

#include "PS3GameModeS3.generated.h"

class APlayerController;
class URandomCollisionTrapComponent;
class UFakeDeathTrapComponent;
class APS3PlayerState;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnRandomVisibleResultsChanged, const TArray<bool>&, TArray<TObjectPtr<UFakeDeathTrapComponent>>);

UCLASS()
class PUZZLESTAY3_API APS3GameModeS3 : public APS3GameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	void NotifyVoiceObjectHeldStateChanged();
private:
	
	void CheckVoiceObjectHeldPlayerCount();
	
	void ChangeVoiceChatStateAndBlockingVolumeCollision(int32 Count);
	
	void SetAllPlayersVoiceChatState(EVoiceChatState NewState);

	void ApplyVoiceChatStateToPlayer(APS3PlayerState* PlayerState);
	void TryMakeRandomVisibleResults();

protected:
	virtual bool StageRestartIfPlayerDead() const override { return true; }
	
	void MakeRandomVisibleResults();
	
	// true  = P1이 Visible, P2는 Invisible
	// false = P1이 Invisible, P2는 Visible
	TArray<bool> RandomVisibleResult;
	TArray<TArray<bool>> RandomVisibleResults;
	
	TArray<TObjectPtr<UFakeDeathTrapComponent>> FakeDeathTrapComponents;
	TArray<TObjectPtr<APlayerController>> PlayerControllers;
	
public:
	
	FOnRandomVisibleResultsChanged OnRandomVisibleResultsChanged;
	
	void RegisterFakeDeathTrapComponent(UFakeDeathTrapComponent* FakeDeathTrapComponent);
	void UnregisterFakeDeathTrapComponent(UFakeDeathTrapComponent* FakeDeathTrapComponent);
	
	void RegisterPlayerController(APlayerController* PlayerController);
	void UnregisterPlayerController(APlayerController* PlayerController);
	
	void BroadcastRandomVisibleResults();
	
private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class US3_GameRuleDataAsset> S3_GameRuleDataAsset;
	
	int32 MaxPlayer;
	int32 MaxFakeDeathTrap;

};

