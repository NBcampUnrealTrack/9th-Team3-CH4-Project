// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameStateS3.h"

#include "Net/UnrealNetwork.h"

void APS3GameStateS3::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APS3GameStateS3, bStage3BlockingVolumeDisabled);
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