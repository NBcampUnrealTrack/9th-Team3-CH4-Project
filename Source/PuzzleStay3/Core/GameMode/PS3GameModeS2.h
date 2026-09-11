
#pragma once

#include "CoreMinimal.h"
#include "PS3GameModeBase.h"
#include "PS3GameModeS2.generated.h"

class URandomCollisionTrapComponent;

UCLASS()
class PUZZLESTAY3_API APS3GameModeS2 : public APS3GameModeBase
{
	GENERATED_BODY()

public:
	
	virtual void BeginPlay() override;
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
protected:
	virtual void InitializeToDataAssets() override;
	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	TObjectPtr<class US2_GameRuleDataAsset> S2_GameRuleDataAsset;
	
protected:	
	//플레이어가 생명 0개 되면 델리게이트 -> 모드가 듣고 리스폰 처리
	void RegisterPlayerLifeCountState(APS3PlayerState* PS3PlayerState);
	
	UFUNCTION()
	void ReSpawnPlayer(APlayerController* TargetPlayerController);
	void UnPossessedAndDestroyOldPawn(APlayerController* TargetPlayerController);
	
	virtual bool StageRestartIfPlayerDead() const override { return true; }

	void MakeRandomCollisionResults();

	// true  = 왼쪽이 BlockAll, 오른쪽이 NoCollision
	// false = 왼쪽이 NoCollision, 오른쪽이 BlockAll
	TArray<bool> RandomCollisionResults;
	TArray<bool> RandomCollisionLayoutResults;
	//월드의 발판액터 개수 확인용
	TArray<TObjectPtr<URandomCollisionTrapComponent>> RandomCollisionTrapComponent;

public:
	void RegisterRandomCollisionTrapCompo(URandomCollisionTrapComponent* TrapComponent);
	void UnregisterRandomCollisionTrapCompo(URandomCollisionTrapComponent* TrapComponent);

	TArray<bool> GetRandomCollisionLayoutResults();
};


