// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameModeS5.h"

#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerState.h"
#include "PuzzleStay3/Core/GameInstance/PS3GameInstance.h"
#include "PuzzleStay3/Player/Controller/PS3ChoiceController.h"

void APS3GameModeS5::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(GameLimitTimeHandle, this, &ThisClass::OnReduceGameTime, 1.f, true);
}

void APS3GameModeS5::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	SetPlayerRole(NewPlayer);
}



void APS3GameModeS5::OnReduceGameTime()
{
	float CurrentGameLimitTime = GameLimitTime;
	
	--CurrentGameLimitTime;
	
	if (CurrentGameLimitTime <= 0.0f)
	{
		//TODO 나중에 게임모드베이스에서 GameOver 함수 추가하기
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, 
			FString::Printf(TEXT("GameOver 예정")));
	}
}

void APS3GameModeS5::OnGameStart()
{
	
	
}


void APS3GameModeS5::SetPlayerRole(const APlayerController* CurrentPlayer)
{
	if (IsValid(CurrentPlayer) == false) return;
	if (IsValid(CurrentPlayer->PlayerState) == false) return;
	
	const FUniqueNetIdRepl& PlayerID = CurrentPlayer->PlayerState->GetUniqueId();
	if (PlayerID.IsValid() == false) return;
	
	auto* PS3GameInstance = Cast<UPS3GameInstance>(GetGameInstance());
	if (IsValid(PS3GameInstance) == false) return; 
	
	EPS3PlayerRoleType CurrentPlayerRoleType = PS3GameInstance->GetControllerEType(PlayerID);
	
	if (CurrentPlayerRoleType == EPS3PlayerRoleType::PlayerRole_ThirdPerson)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green,
		FString::Printf(TEXT("3인칭조작 컨트롤러 할당 해야함")));
	}
	
	if (CurrentPlayerRoleType == EPS3PlayerRoleType::PlayerRole_Screen)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue,
			FString::Printf(TEXT("스크린조작 컨트롤러 할당 해야함")));
	}	
	
	if (CurrentPlayerRoleType == EPS3PlayerRoleType::PlayerRole_None)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red,
	FString::Printf(TEXT("UI 컨트롤러 할당 해야함")));
	}
}
