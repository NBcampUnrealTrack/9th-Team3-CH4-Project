// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameModeS5.h"

void APS3GameModeS5::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(GameLimitTimeHandle, this, &ThisClass::OnReduceGameTime, 1.f, true);
}


void APS3GameModeS5::OnReduceGameTime()
{
	float CurrentGameLimitTime = GameLimitTime;
	
	--CurrentGameLimitTime;
	
	if (CurrentGameLimitTime <= 0.0f)
	{
		//TODO 나중에 GameOver 함수 추가하기
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, 
			FString::Printf(TEXT("GameOver 예정")));
	}
}

