// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameStateS2.h"

#include "Data/DataAsset/S2_GameRuleDataAsset.h"
#include "Data/Delegates/UIDelegatesSubsystem.h"

void APS3GameStateS2::BeginPlay()
{
	Super::BeginPlay();
	
	SetUIMacroTimerHandle(
		StageType_UI_DelayTime,
		[this]() { PS3_BROADCAST_TO_MVVM_OneParams(OnStageType_UI, S2_GameRuleDataAsset->StageType_S2); });

	SetUIMacroTimerHandle(
		TextNotify_UI_DelayTime,
		[this]() { PS3_BROADCAST_TO_MVVM_OneParams(OnTextNotify_UI, S2_GameRuleDataAsset->TextNotifyTypeForUI_S2); });
}

void APS3GameStateS2::InitializeToDataAssets()
{
	Super::InitializeToDataAssets();
	
	checkf(IsValid(S2_GameRuleDataAsset) == true, TEXT("[APS3GameModeS2]의 데이터에셋이 비어있습니다."));
	
}