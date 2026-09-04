// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PS3GameModeBase.h"
#include "Data/Enum/VoiceChatState.h"
#include "PS3GameModeS3.generated.h"

class UFakeDeathTrapComponent;
class APS3PlayerState;

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

protected:
	virtual bool StageRestartIfPlayerDead() const override { return true; }
	
	void MakeRandomVisibleResults();
	
	// true  = P1이 Visible, P2는 Invisible
	// false = P1이 Invisible, P2는 Visible
	bool RandomVisibleResults = true;

public:
	bool GetRandomVisibleResults();
};
