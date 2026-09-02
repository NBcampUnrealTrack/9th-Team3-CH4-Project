// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameState.h"

#include "Net/UnrealNetwork.h"


void APS3GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, GameLimitTime);
}
