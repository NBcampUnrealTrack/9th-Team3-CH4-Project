// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameStateS1.h"

#include "Data/DataAsset/S1_GameRuleDataAsset.h"
#include "Data/Delegates/UIDelegatesSubsystem.h"
#include "Net/UnrealNetwork.h"

void APS3GameStateS1::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APS3GameStateS1, bStage1BlockingVolumeDisabled);
}

void APS3GameStateS1::BeginPlay()
{
	Super::BeginPlay();
		
	SetUIMacroTimerHandle(
		StageType_UI_DelayTime,
		[this]() { PS3_BROADCAST_TO_MVVM_OneParams(OnStageType_UI, S1_GameRuleDataAsset->StageType_S1); });
	
	SetUIMacroTimerHandle(
		TextNotify_UI_DelayTime,
		[this]() { PS3_BROADCAST_TO_MVVM_OneParams(OnTextNotify_UI, S1_GameRuleDataAsset->TextNotifyTypeForUI_S1); });
	
	SetUIMacroTimerHandle(
		Tutorial_UI_DelayTime,
		[this]() { PS3_BROADCAST_TO_MVVM_OneParams(OnTutorial_UI,true); });
}

void APS3GameStateS1::InitializeToDataAssets()
{
	Super::InitializeToDataAssets();
	
	checkf(IsValid(S1_GameRuleDataAsset) == true, TEXT("[APS3GameModeS1]의 데이터에셋이 비어있습니다."));
	
	Tutorial_UI_DelayTime = S1_GameRuleDataAsset->Tutorial_UI_DelayTime;
	
}

void APS3GameStateS1::SetStage1BlockingVolumeDisabled(bool bDisabled)
{
	if (!HasAuthority()) return;
	if (bStage1BlockingVolumeDisabled == bDisabled) return;

	bStage1BlockingVolumeDisabled = bDisabled;
	OnRep_Stage1BlockingVolumeDisabled();
}

void APS3GameStateS1::OnRep_Stage1BlockingVolumeDisabled()
{
	OnStage1BlockingVolumeDisabled.Broadcast(bStage1BlockingVolumeDisabled);
}
