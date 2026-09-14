// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameStateBase.h"

#include "Data/DataAsset/Base_GameRuleDataAsset.h"
#include "Net/UnrealNetwork.h"

APS3GameStateBase::APS3GameStateBase()
{
	bReplicates = true;
}

void APS3GameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APS3GameStateBase, bEscapeDoorOpened);
}

void APS3GameStateBase::InitializeToDataAssets()
{
	checkf(IsValid(Base_GameRuleDataAsset) == true, TEXT("[APS3GameModeS1]의 데이터에셋이 비어있습니다."));
	
	
	StageType_UI_DelayTime = Base_GameRuleDataAsset->StageType_UI_DelayTime;
	
	TextNotify_UI_DelayTime = Base_GameRuleDataAsset->TextNotify_UI_DelayTime;
}

void APS3GameStateBase::SetEscapeDoorOpened(bool bOpened)
{
	if (!HasAuthority()) return;
	if (bEscapeDoorOpened == bOpened) return;

	bEscapeDoorOpened = bOpened;

	// 서버는 OnRep가 자동 호출되지 않으므로 직접 호출
	OnRep_EscapeDoorOpened();
}

void APS3GameStateBase::OnRep_EscapeDoorOpened()
{
	OnEscapeDoorOpened.Broadcast(bEscapeDoorOpened);
}

void APS3GameStateBase::SetUIMacroTimerHandle(float Timer, TFunction<void()> Callback)
{
	FTimerHandle TimerHandle;
	
	GetWorldTimerManager().SetTimer(
		TimerHandle,
		FTimerDelegate::CreateLambda([Callback](){ Callback(); }),
		Timer,
		false
	);
}
