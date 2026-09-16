// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameStateEnd.h"

#include "Data/DataAsset/End_GameRuleDataAsset.h"
#include "Data/Delegates/UIDelegatesSubsystem.h"

void APS3GameStateEnd::BeginPlay()
{
	Super::BeginPlay();
	
	PS3_UIDELEGATE_TIMER_FOR_MACRO(
			PS3_BROADCAST_TO_MVVM_OneParams(OnTextNotifyVisible_UI,true));
	
	SetUIMacroTimerHandle(
		2.0,
		[this]() { PS3_BROADCAST_TO_MVVM_OneParams(OnTextNotify_UI, End_GameRuleDataAsset->TextNotifyTypeForUI_End); });
}

void APS3GameStateEnd::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void APS3GameStateEnd::InitializeToDataAssets()
{
	Super::InitializeToDataAssets();
	
	checkf(IsValid(End_GameRuleDataAsset) == true, TEXT("[APS3GameStateEnd]의 데이터에셋이 비어있습니다."));
}
