// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameModeEnd.h"

#include "Data/DataAsset/End_GameRuleDataAsset.h"
#include "GameFramework/PlayerState.h"


void APS3GameModeEnd::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeToDataAssets();
}

void APS3GameModeEnd::InitializeToDataAssets()
{
	Super::InitializeToDataAssets();
	
	if(IsValid(End_GameRuleDataAsset) == false) return;
	if (End_GameRuleDataAsset->EndingVideoLevel.IsNull()) return;
	
	EndingVideoLevelPath = End_GameRuleDataAsset->EndingVideoLevel.ToSoftObjectPath().GetLongPackageName();

	RequiredPlayerCount = End_GameRuleDataAsset->RequiredPlayerCount;
}

bool APS3GameModeEnd::CanInteractFinalTreasure(APlayerState* PlayerState) const
{
	if (bEndingTriggered){return false;}

	if (!IsValid(PlayerState)){return false;}

	return !InteractedPlayers.Contains(PlayerState);
}

bool APS3GameModeEnd::RegisterFinalTreasureInteraction(APlayerState* PlayerState)
{
	if (!HasAuthority()) {return false;}

	if (!CanInteractFinalTreasure(PlayerState)) {return false;}

	InteractedPlayers.Add(PlayerState);

	UE_LOG(LogTemp, Warning, TEXT("[FinalTreasure] Interacted %d / %d"),InteractedPlayers.Num(),RequiredPlayerCount);

	if (InteractedPlayers.Num() >= RequiredPlayerCount)
	{
		bEndingTriggered = true;
		TravelToEndingVideoLevel();
	}

	return true;
}

void APS3GameModeEnd::TravelToEndingVideoLevel()
{
	if (EndingVideoLevelPath.IsEmpty()) return;

	GetWorld()->ServerTravel(EndingVideoLevelPath);
}
