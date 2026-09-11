// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameModeS4.h"

#include "Core/GameState/PS3GameStateS4.h"
#include "Data/DataAsset/S4_GameRuleDataAsset.h"
#include "Data/Enum/DoorType.h"

void APS3GameModeS4::BeginPlay()
{
	Super::BeginPlay();
	
	const int32 RandomIndex = FMath::RandRange(0, SubstituteFixedObjectWeight.Num() - 1);

	FixedObjectWeight = SubstituteFixedObjectWeight[RandomIndex];
	
	// 저울 반대편에 올려야 하는 덤벨 합계
	TargetBalancedWeight = FixedObjectWeight - (Player1Weight + Player2Weight);

	APS3GameStateS4* GS = GetGameState<APS3GameStateS4>();
	if (IsValid(GS))
	{
		GS->SetStage4Weights(FixedObjectWeight, TargetBalancedWeight);
	}
}

void APS3GameModeS4::InitializeToDataAssets()
{
	Super::InitializeToDataAssets();
	
	if(IsValid(S4_GameRuleDataAsset) == false) return;
	if (S4_GameRuleDataAsset->NextStageLevel.IsNull()) return;
	
	NextStageLevelPath = S4_GameRuleDataAsset->NextStageLevel.ToSoftObjectPath().GetLongPackageName();
	
	SubstituteFixedObjectWeight = S4_GameRuleDataAsset->SubstituteFixedObjectWeight;

	Player1Weight = S4_GameRuleDataAsset->Player1Weight;

	Player2Weight = S4_GameRuleDataAsset->Player2Weight;
}


//저울이 정답 판별 후 이 함수 호출
void APS3GameModeS4::NotifyJeoulResult(bool bIsSuccess)
{
	if (!bIsSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Stage4] Jeoul result: Wrong"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Stage4] Jeoul result: Correct"));

	OpenStage4FirstDoor();
}

void APS3GameModeS4::OpenStage4FirstDoor()
{
	APS3GameStateS4* GS = GetGameState<APS3GameStateS4>();
	if (!IsValid(GS)) return;

	GS->SetStage4FirstDoorOpened(true);
}

