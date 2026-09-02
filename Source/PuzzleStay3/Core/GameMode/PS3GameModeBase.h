#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "PS3GameModeBase.generated.h"

class UInteractionSwitchComponent;
class ADoor;

//구현 완료
//DisableBlockingVolumeCompo가 호출할 함수
//Stage3BlockingVolumeCompo - 스테이지3 못가게 가로막는 블록볼륨
//6.게임 오버 시 스테이지 재 시작하는 함수 StageRestart() -> 가상 함수
//스테이지 클리어시 다음 스테이지 오픈하는 가상함수

//구현예정
// Stage3 블록볼륨한테 지시 내리는 내용

//보류
//5.스테이지 이동하는 탈출 문에 사용되는B기믹
// b기믹1 <-p1 활성화! p1 enum = IsInteracting 
// b기믹 활성화 로직에서 if ( player enum == NowInteracting) { b기믹과 상호작용 불가능!}
// else { b기믹과 상호작용 가능}
//p2 enum = NotIntertacting
//b기믹2



UENUM(BlueprintType)
enum class EPS3StageNumber : uint8
{
	Stage1,
	Stage3
};

UENUM(BlueprintType)
enum class ERandomCollisionState : uint8
{
	NoCollision,
	BlockAll
};

DECLARE_MULTICAST_DELEGATE(FOnEscapeDoorOpened);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBlockingVolumeDisabled, EPS3StageNumber);

UCLASS()
class PUZZLESTAY3_API APS3GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

#pragma region InteractionSwitch
public:
	//b기믹스위치 스위치 개수 저장
	void RegisterInteractionSwitch(UInteractionSwitchComponent* SwitchComp);
	void UnregisterInteractionSwitch(UInteractionSwitchComponent* SwitchComp);

	//모든 b기믹스위치 활성화 여부 확인
	bool AllInteractionSwitchActivated() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameRule|InteractionSwitch")
	TArray<TObjectPtr<UInteractionSwitchComponent>> InteractionSwitches;

private:
	UFUNCTION()
	void HandleSwitchActivatedChanged(bool bActivated);

	FDelegateHandle InteractionSwitchCompoHandle;

#pragma endregion

#pragma region OpenDoor
public:
	//문열기 델리게이트
	FOnEscapeDoorOpened OnEscapeDoorOpened;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameRule")
	bool bEscapeDoorOpened = false;

private:
	void OpenEscapeDoor();

#pragma endregion
	
#pragma region BlokingVolume
public:
	//블록볼륨 델리게이트
	FOnBlockingVolumeDisabled OnBlockingVolumeDisabled;

	virtual void DisableBlockingVolume(EPS3StageNumber StageNumber);
	
#pragma endregion
	
#pragma region StageRestart
	//게임오버 시 해당 스테이지 재오픈
	virtual void StageRestart() ;
	
#pragma endregion
	
#pragma region StageClear
	//각 스테이지 클리어 시 사용할 가상함수
	virtual void StageClear();
#pragma endregion
};