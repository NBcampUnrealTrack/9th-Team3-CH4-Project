// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameStateS4.h"

#include "Data/DataAsset/S4_GameRuleDataAsset.h"
#include "Data/Delegates/UIDelegatesSubsystem.h"
#include "Net/UnrealNetwork.h"


void APS3GameStateS4::BeginPlay()
{
	Super::BeginPlay();
	
	SetUIMacroTimerHandle(
	[this]() { PS3_BROADCAST_TO_MVVM_OneParams(OnStageType_UI, S4_GameRuleDataAsset->StageType_S4); });
	
	SetUIMacroTimerHandle(
	[this]() {PS3_BROADCAST_TO_MVVM_OneParams(OnTextNotify_UI, EPS3TextNotifyType::Stage4);});
}

void APS3GameStateS4::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APS3GameStateS4, FixedObjectWeight);
	DOREPLIFETIME(APS3GameStateS4, TargetBalancedWeight);
	DOREPLIFETIME(APS3GameStateS4, bStage4FirstDoorOpened);
}

void APS3GameStateS4::SetStage4Weights(float NewFixedWeight, float NewTargetWeight)
{
	if (!HasAuthority()) return;

	FixedObjectWeight = NewFixedWeight;
	TargetBalancedWeight = NewTargetWeight;

	OnRep_Stage4Weights();
}

void APS3GameStateS4::OnRep_Stage4Weights()
{
	OnStage4WeightChanged.Broadcast(FixedObjectWeight, TargetBalancedWeight);
}

void APS3GameStateS4::SetStage4FirstDoorOpened(bool bOpened)
{
	if (!HasAuthority()) return;
	if (bStage4FirstDoorOpened == bOpened) return;

	bStage4FirstDoorOpened = bOpened;
	
	OnRep_Stage4FirstDoorOpened();
}

void APS3GameStateS4::OnRep_Stage4FirstDoorOpened()
{
	OnStage4FirstDoorOpenedChanged.Broadcast(bStage4FirstDoorOpened);
}