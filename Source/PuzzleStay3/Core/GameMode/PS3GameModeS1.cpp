// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameModeS1.h"

#include "Core/GameState/PS3GameStateS1.h"


void APS3GameModeS1::DisableBlockingVolume(EPS3StageNumber StageNumber)
{
	if (StageNumber != EPS3StageNumber::Stage1) return;

	APS3GameStateS1* GS = GetGameState<APS3GameStateS1>();
	if (!IsValid(GS)) return;

	GS->SetStage1BlockingVolumeDisabled(true);
}
