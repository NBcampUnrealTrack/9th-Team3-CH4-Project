// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PS3GameModeBase.h"
#include "PS3GameModeS2.generated.h"

//스테이지 (재)시작 시 발판 Collision 랜덤값 생성 및 게임인스턴스에 저장 - 보류

class URandomCollisionTrapComponent;

UCLASS()
class PUZZLESTAY3_API APS3GameModeS2 : public APS3GameModeBase
{
	GENERATED_BODY()
	
	protected:
	void BeginPlay() override;
	
	virtual bool StageRestartIfPlayerDead() const override { return true; }
	
	void MakeRandomCollisionResults();
	
	// true  = 왼쪽이 BlockAll, 오른쪽이 NoCollision
	// false = 왼쪽이 NoCollision, 오른쪽이 BlockAll
	TArray<bool> RandomCollisionResults;
	
	//월드의 발판액터 개수 확인용
	TArray<TObjectPtr<URandomCollisionTrapComponent>> RandomCollisionTrapComponent;

public:
	void RegisterRandomCollisionTrapCompo(URandomCollisionTrapComponent* TrapComponent);
	void UnregisterRandomCollisionTrapCompo(URandomCollisionTrapComponent* TrapCompo);
	
	bool GetRandomCollisionResult(int32 RowIndex);
};
