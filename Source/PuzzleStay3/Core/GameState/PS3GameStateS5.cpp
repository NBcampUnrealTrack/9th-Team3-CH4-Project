// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameStateS5.h"

#include "Core/GameMode/PS3GameModeS5.h"
#include "Data/DataAsset/S5_GameRuleDataAsset.h"
#include "Net/UnrealNetwork.h"


APS3GameStateS5::APS3GameStateS5()
{
	bReplicates = true;
	
	
}


void APS3GameStateS5::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(GetCastPS3GameModeS5()) == false) return;
	GameLimitTime = S5_GameRuleDataAsset->MaxGameLimitTime;
}


void APS3GameStateS5::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, GameLimitTime);
	DOREPLIFETIME(ThisClass, bIsGameOver);
	DOREPLIFETIME(ThisClass, bIsSelectedFieldType);
	DOREPLIFETIME(ThisClass, bIsSelectedScreenType);
}


void APS3GameStateS5::ReSpawnPlayer(APlayerController* TargetPlayerController)
{
	if (HasAuthority() == true)
	{
		if (IsValid(TargetPlayerController) == false) return;
		if (IsValid(CastPS3GameModeS5) == false) return;
		
		CastPS3GameModeS5->ReSpawnPlayer(TargetPlayerController);
	}
}


void APS3GameStateS5::OnRep_IsGameOver()
{
	if (bIsGameOver == false) return;
	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (IsValid(PlayerController))
	{
		FInputModeUIOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = true;
	}
	
	/*TODO 결론적으로 게임오버가 되면 이쪽으로 넘어온다.
	즉, 이때 클라이언트 화면에 UI 띄우기, 입력 모드 전환 등 Local 처리해야함
	(현준님과 상의하기)*/
	UE_LOG(LogTemp, Warning, TEXT("게임 종료 UI 띄어야함. MVVM현준님과 상의하기"));
}


void APS3GameStateS5::OnRep_GameLimitTime()
{
	//TODO 남은 제한시간 UI 업데이트 함수 구현하기 / 쓰러지고 일어나는 몽타주나 폭발 특수효과?
	UE_LOG(LogTemp, Error, TEXT("(UI표시 업데이트 예정) 남은 제한시간: %f"), GameLimitTime);
	
}


void APS3GameStateS5::SetDeductGameLimitTime_AuthorityOnRep(float TimeToDeducted)
{
	if (HasAuthority() == true)
	{
		GameLimitTime = GameLimitTime - TimeToDeducted;
		OnRep_GameLimitTime(); //서버가 읽을때는 수동으로 OnRep를 써야함
	}
	
}


void APS3GameStateS5::SetIsGameOver_AuthorityOnRep(bool SetIsGameOver)
{
	if (HasAuthority() == true)
	{
		bIsGameOver = SetIsGameOver;
		OnRep_IsGameOver();
	}
}


void APS3GameStateS5::OnGameOver()
{
	SetIsGameOver_AuthorityOnRep(true);
}


void APS3GameStateS5::OnReduceGameTime(float ReducedTimeRange)
{
	SetDeductGameLimitTime_AuthorityOnRep(ReducedTimeRange);
}


void APS3GameStateS5::OnTimeDeduction(float TimeToDeducted)
{
	SetDeductGameLimitTime_AuthorityOnRep(TimeToDeducted);
}


void APS3GameStateS5::StageRestart()
{
	if (IsValid(CastPS3GameModeS5) == false) return;
	
	CastPS3GameModeS5->StageRestart();
}


void APS3GameStateS5::OnQuitGame()
{
	APlayerController* CurrentPlayer = GetWorld()->GetFirstPlayerController();
	if (CurrentPlayer == nullptr) return;
	
	if (IsValid(CastPS3GameModeS5) == false) return;
		
	CastPS3GameModeS5->OnQuitGame();
}

APS3GameModeS5* APS3GameStateS5::GetCastPS3GameModeS5()
{
	if (HasAuthority() == true)
	{
		if (IsValid(CastPS3GameModeS5)) return CastPS3GameModeS5;

		if (UWorld* World = GetWorld())
		{
			CastPS3GameModeS5 = Cast<APS3GameModeS5>(World->GetAuthGameMode());
			return CastPS3GameModeS5;
		}
	}
	
	return nullptr;
}


