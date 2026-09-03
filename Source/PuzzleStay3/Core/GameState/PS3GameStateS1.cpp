// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameStateS1.h"

#include "Net/UnrealNetwork.h"

void APS3GameStateS1::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APS3GameStateS1, bStage1BlockingVolumeDisabled);
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