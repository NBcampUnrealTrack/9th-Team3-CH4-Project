#include "CheatManager/PS3CheatManager.h"

#include "Core/GameMode/PS3GameModeBase.h"
#include "Core/GameMode/PS3GameModeS2.h"
#include "Core/GameMode/PS3GameModeS5.h"
#include "Core/GameState/PS3GameStateS4.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

void UPS3CheatManager::PS3NextStage()
{
	APS3GameModeBase* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<APS3GameModeBase>() : nullptr;
	if (!IsValid(GameMode))
	{
		PrintCheatMessage(TEXT("[Cheat] GameMode 권한이 없어 다음 스테이지로 이동할 수 없습니다."));
		return;
	}

	PrintCheatMessage(TEXT("[Cheat] 다음 스테이지로 이동합니다."));
	GameMode->GoToNextStageForCheat();
}

void UPS3CheatManager::PS3RestartStage()
{
	APS3GameModeBase* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<APS3GameModeBase>() : nullptr;
	if (!IsValid(GameMode))
	{
		PrintCheatMessage(TEXT("[Cheat] GameMode 권한이 없어 현재 스테이지를 재시작할 수 없습니다."));
		return;
	}

	PrintCheatMessage(TEXT("[Cheat] 현재 스테이지를 재시작합니다."));
	GameMode->StageRestart();
}

void UPS3CheatManager::PS3ShowStage2Layout()
{
	const APS3GameModeS2* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<APS3GameModeS2>() : nullptr;
	if (!IsValid(GameMode))
	{
		PrintCheatMessage(TEXT("[Cheat] 현재 GameMode가 Stage2가 아닙니다."));
		return;
	}

	const TArray<bool> Results = GameMode->GetRandomCollisionLayoutResults();
	if (Results.IsEmpty())
	{
		PrintCheatMessage(TEXT("[Cheat] Stage2 랜덤 배열이 비어있습니다."));
		return;
	}

	TArray<FString> ResultTexts;
	ResultTexts.Reserve(Results.Num());

	for (int32 Index = 0; Index < Results.Num(); ++Index)
	{
		ResultTexts.Add(FString::Printf(TEXT("%d:%s"), Index + 1, Results[Index] ? TEXT("BlockAll") : TEXT("NoCollision")));
	}

	PrintCheatMessage(FString::Printf(TEXT("[Cheat] Stage2 랜덤 배열: %s"), *FString::Join(ResultTexts, TEXT(", "))));
}

void UPS3CheatManager::PS3ShowStage4WeightAnswer()
{
	const APS3GameStateS4* GameState = GetWorld() ? GetWorld()->GetGameState<APS3GameStateS4>() : nullptr;
	if (!IsValid(GameState))
	{
		PrintCheatMessage(TEXT("[Cheat] 현재 GameState가 Stage4가 아닙니다."));
		return;
	}

	PrintCheatMessage(FString::Printf(
		TEXT("[Cheat] Stage4 무게 정답: 고정 무게 %.0f, 맞춰야 할 덤벨 합계 %.0f"),
		GameState->GetFixedObjectWeight(),
		GameState->GetTargetBalancedWeight()));
}

void UPS3CheatManager::PS3ShowStage5RealSwitches()
{
	const APS3GameModeS5* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<APS3GameModeS5>() : nullptr;
	if (!IsValid(GameMode))
	{
		PrintCheatMessage(TEXT("[Cheat] 현재 GameMode가 Stage5가 아닙니다."));
		return;
	}

	const TArray<FString> Labels = GameMode->GetRealInteractionGimmickLabelsForCheat();
	if (Labels.IsEmpty())
	{
		PrintCheatMessage(TEXT("[Cheat] Stage5 진짜 interaction gimmick을 찾지 못했습니다."));
		return;
	}

	PrintCheatMessage(FString::Printf(TEXT("[Cheat] Stage5 진짜 interaction switch 번호: %s"), *FString::Join(Labels, TEXT(", "))));
}

void UPS3CheatManager::PrintCheatMessage(const FString& Message) const
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Yellow, Message);
	}
}
