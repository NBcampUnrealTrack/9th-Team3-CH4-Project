// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameStateS2.h"

#include "Data/DataAsset/S2_GameRuleDataAsset.h"
#include "Data/Delegates/UIDelegatesSubsystem.h"
#include "Net/UnrealNetwork.h"

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

void APS3GameStateS2::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APS3GameStateS2, bPlayerDeadGameOver);
}

void APS3GameStateS2::NotifyPlayerDeadGameOver()
{
	if (!HasAuthority()) return;
	if (bPlayerDeadGameOver) return;

	bPlayerDeadGameOver = true;

	// 서버는 OnRep 자동 호출 안 됨
	OnRep_PlayerDeadGameOver();
}

void APS3GameStateS2::OnRep_PlayerDeadGameOver()
{
	if (!bPlayerDeadGameOver) return;

	PS3_BROADCAST_TO_MVVM_OneParams(OnIsGameOver_UI, true);
}
