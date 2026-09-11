// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameModeS1.h"

#include "Core/GameState/PS3GameStateS1.h"
#include "Data/DataAsset/S1_GameRuleDataAsset.h"


APS3GameModeS1::APS3GameModeS1()
{
	SetInteractionSwitchTimerUsed(true);
}

void APS3GameModeS1::BeginPlay()
{
	Super::BeginPlay();
	
}

void APS3GameModeS1::DisableBlockingVolume(EPS3StageNumber StageNumber)
{
	if (StageNumber != EPS3StageNumber::Stage1) return;

	APS3GameStateS1* GS = GetGameState<APS3GameStateS1>();
	if (!IsValid(GS)) return;

	GS->SetStage1BlockingVolumeDisabled(true);
}

void APS3GameModeS1::InitializeToDataAssets()
{
	Super::InitializeToDataAssets();
	
	if(IsValid(S1_GameRuleDataAsset) == false) return;
	if (S1_GameRuleDataAsset->NextStageLevel.IsNull()) return;
	
	NextStageLevelPath = S1_GameRuleDataAsset->NextStageLevel.ToSoftObjectPath().GetLongPackageName();
}