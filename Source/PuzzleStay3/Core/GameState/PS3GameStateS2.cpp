// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameStateS2.h"

#include "Data/DataAsset/S2_GameRuleDataAsset.h"
#include "Data/Delegates/UIDelegatesSubsystem.h"

void APS3GameStateS2::BeginPlay()
{
	Super::BeginPlay();
	
	SetUIMacroTimerHandle(
		[this]() { PS3_BROADCAST_TO_MVVM_OneParams(OnStageType_UI, S2_GameRuleDataAsset->StageType_S2); });
}
