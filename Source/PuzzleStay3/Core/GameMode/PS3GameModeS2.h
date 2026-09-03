// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PS3GameModeBase.h"
#include "PS3GameModeS2.generated.h"

//스테이지 (재)시작 시 발판 Collision 랜덤값 생성 및 게임인스턴스에 저장 - 보류

UCLASS()
class PUZZLESTAY3_API APS3GameModeS2 : public APS3GameModeBase
{
	GENERATED_BODY()
	
	protected:
	virtual bool StageRestartIfPlayerDead() const override { return true; }
};
