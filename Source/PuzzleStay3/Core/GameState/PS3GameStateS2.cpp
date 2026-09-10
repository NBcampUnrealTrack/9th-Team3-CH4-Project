// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameStateS2.h"

#include "Data/DataAsset/S2_GameRuleDataAsset.h"
#include "Data/Delegates/UIDelegatesSubsystem.h"

void APS3GameStateS2::BeginPlay()
{
	Super::BeginPlay();

	InitializeToDataAssets();
	
	SetUIMacroTimerHandle(
		0.01,
		[this]() { PS3_BROADCAST_TO_MVVM_OneParams(OnStageType_UI, S2_GameRuleDataAsset->StageType_S2); });

	SetUIMacroTimerHandle(
		0.5,
		[this]() { PS3_BROADCAST_TO_MVVM_OneParams(OnTextNotify_UI, EPS3TextNotifyType::Stage2); });
}

void APS3GameStateS2::InitializeToDataAssets()
{
	checkf(IsValid(S2_GameRuleDataAsset) == true, TEXT("[APS3GameModeS2]의 데이터에셋이 비어있습니다."));
}