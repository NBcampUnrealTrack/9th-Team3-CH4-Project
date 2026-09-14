// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameStateS3.h"

#include "Data/DataAsset/S3_GameRuleDataAsset.h"
#include "Data/Delegates/UIDelegatesSubsystem.h"
#include "Net/UnrealNetwork.h"

void APS3GameStateS3::BeginPlay()
{
	Super::BeginPlay();
	
	SetUIMacroTimerHandle(
		StageType_UI_DelayTime,
		[this]() { PS3_BROADCAST_TO_MVVM_OneParams(OnStageType_UI, S3_GameRuleDataAsset->StageType_S3); });

	SetUIMacroTimerHandle(
		TextNotify_UI_DelayTime,
		[this]() { PS3_BROADCAST_TO_MVVM_OneParams(OnTextNotify_UI, S3_GameRuleDataAsset->TextNotifyTypeForUI_S3); });
}

void APS3GameStateS3::InitializeToDataAssets()
{
	checkf(IsValid(S3_GameRuleDataAsset) == true, TEXT("[APS3GameModeS3]의 데이터에셋이 비어있습니다."));
	
}

void APS3GameStateS3::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APS3GameStateS3, bStage3BlockingVolumeDisabled);
	DOREPLIFETIME(APS3GameStateS3, bStage3VoiceChatActivated);
}

void APS3GameStateS3::SetStage3BlockingVolumeDisabled(bool bDisabled)
{
	if (!HasAuthority()) return;
	if (bStage3BlockingVolumeDisabled == bDisabled) return;

	bStage3BlockingVolumeDisabled = bDisabled;
	OnRep_Stage3BlockingVolumeDisabled();
}

void APS3GameStateS3::OnRep_Stage3BlockingVolumeDisabled()
{
	OnStage3BlockingVolumeDisabled.Broadcast(bStage3BlockingVolumeDisabled);
}

void APS3GameStateS3::SetStage3VoiceChatActivated(bool bActivated)
{
	if (!HasAuthority()) return;
	if (bStage3VoiceChatActivated == bActivated) return;

	bStage3VoiceChatActivated = bActivated;
	OnRep_Stage3VoiceChatActivated();
}

void APS3GameStateS3::OnRep_Stage3VoiceChatActivated()
{
	OnStage3VoiceChatActivated.Broadcast(bStage3VoiceChatActivated);
}
