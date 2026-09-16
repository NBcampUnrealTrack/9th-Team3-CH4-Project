#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "PS3CheatManager.generated.h"

UCLASS()
class PUZZLESTAY3_API UPS3CheatManager : public UCheatManager
{
	GENERATED_BODY()

	//CTRL + ALT + 1 (PS3NextStage)= 다음 스테이지
	//CTRL + ALT + 2 (PS3RestartStage)= 현재 스테이지 재시작
	//CTRL + ALT + 3 (PS3ShowStage2Layout)= Stage2 랜덤 배열 표시
	//CTRL + ALT + 4 (PS3ShowStage4WeightAnswer)= Stage4 무게 정답 표시
	//CTRL + ALT + 5 (PS3ShowStage5RealSwitch)= Stage5 Real Interaction Switch 표시

	
public:
	UFUNCTION(Exec)
	void PS3NextStage();

	UFUNCTION(Exec)
	void PS3RestartStage();

	UFUNCTION(Exec)
	void PS3ShowStage2Layout();

	UFUNCTION(Exec)
	void PS3ShowStage4WeightAnswer();

	UFUNCTION(Exec)
	void PS3ShowStage5RealSwitches();

private:
	void PrintCheatMessage(const FString& Message) const;
};
