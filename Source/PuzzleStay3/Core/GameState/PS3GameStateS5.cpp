// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameStateS5.h"

#include "Core/GameMode/PS3GameModeS5.h"
#include "Net/UnrealNetwork.h"


APS3GameStateS5::APS3GameStateS5()
{
	bReplicates = true;
}

void APS3GameStateS5::BeginPlay()
{
	Super::BeginPlay();
	
}

void APS3GameStateS5::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, GameLimitTime);
	DOREPLIFETIME(ThisClass, bIsGameOver);
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

void APS3GameStateS5::OnGameOver()
{
	if (HasAuthority() == true)
	{
		bIsGameOver = true;
		OnRep_IsGameOver();
	}
}

void APS3GameStateS5::OnReduceGameTime()
{
	if (HasAuthority() == true)
	{
		--GameLimitTime;
		UE_LOG(LogTemp, Error, TEXT("남은 제한시간: %f"), GameLimitTime);
	}
}




