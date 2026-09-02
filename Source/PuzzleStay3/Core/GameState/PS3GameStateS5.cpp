// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameStateBase.h"

#include "Core/GameMode/PS3GameModeS5.h"
#include "Net/UnrealNetwork.h"


void APS3GameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, GameLimitTime);
}


void APS3GameStateBase::BeginPlay()
{
	Super::BeginPlay();
	
	
}


void APS3GameStateBase::OnGameStart()
{
	GetWorld()->GetTimerManager().SetTimer(GameLimitTimeHandle, this, &ThisClass::OnReduceGameTime, 1.f, true);
}

void APS3GameStateBase::OnGameOver()
{
	//TODO 게임오버 UI로 변경
	auto* PS3GameModeS5 = Cast<APS3GameModeS5>(GetWorld()->GetAuthGameMode());
	if (IsValid(PS3GameModeS5) == false) return;
	UE_LOG(LogTemp, Warning, TEXT("게임모드 Cast 완료. OnGameOver() 실행가능."));
		
	PS3GameModeS5->OnGameOver();
}

void APS3GameStateBase::OnReduceGameTime()
{
	--GameLimitTime;
	
	if (GameLimitTime <= 0.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(GameLimitTimeHandle);
		UE_LOG(LogTemp, Warning, TEXT("제한시간 종료."));
		
		bIsGameOver = true;
	}
}


void APS3GameStateBase::OnRep_IsGameOver()
{
	if (bIsGameOver == false) return;
	
	OnGameOver();
	
}
