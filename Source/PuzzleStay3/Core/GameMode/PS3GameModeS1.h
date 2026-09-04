#pragma once

#include "CoreMinimal.h"
#include "PS3GameModeBase.h"
#include "PS3GameModeS1.generated.h"


//현재 탈출문 InteractionSwitch는 월드에 있는 전부가 활성화되었는지 확인하는 로직임
//1스테이지 제외 나머지 스테이지는 각각 플레이어가 1개씩 상호작용하도록 유도해야함 (Player enum IsInteracting 사용)
//1스테이지는 모든 플레이어가  모든 스위치에 중복 상호작용 가능하고 각 스위치마다 20초의 제한시간 후 다시 비활성화 됨

//1스테이지에서는 스위치가 비활성화될 시 활성화 목록에서 제외하는 로직 필요


UCLASS()
class PUZZLESTAY3_API APS3GameModeS1 : public APS3GameModeBase
{
	GENERATED_BODY()

public:
	virtual void DisableBlockingVolume(EPS3StageNumber StageNumber) override;

private:
	bool bInteractionSwitchTimerUsed = true;
};
