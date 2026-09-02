// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PS3GameModeBase.h"
#include "Data/Enum/VoiceChatState.h"
#include "PS3GameModeS3.generated.h"

class UCustomVoiceComponent;

UCLASS()
class PUZZLESTAY3_API APS3GameModeS3 : public APS3GameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	void RegisterCustomVoiceComponent(UCustomVoiceComponent* CustomVoiceComponent);
	
	void HandleVoiceObjectHeld();

private:
	
	void CheckVoiceObjectHeldPlayerCount();
	
	void ChangeVoiceChatStateAndBlockingVolumeCollision(int32 Count);
	
	void SetAllPlayersVoiceChatState(EVoiceChatState NewState);
	
	void StageRestart() override;
	
};
