// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3PlayerControllerBase.h"

#include "Core/GameMode/PS3GamemodeBase.h"
#include "Core/GameMode/PS3GameModeS5.h"
#include "Data/Delegates/UIDelegatesSubsystem.h"
#include "UI/HUD/PlayerHUD.h"
#include "UI/ViewModel/PS3ViewModel.h"

void APS3PlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(InitTimerHandle, this, &ThisClass::ConfigureInputMapping, 0.01f, false);
}


void APS3PlayerControllerBase::OnClickedRestartGameButton()
{
	PS3_BROADCAST_TO_MVVM_OneParams(OnIsGameOver_UI, true);
	
	ServerRPC_OnClickedMainMenuButton();
}


void APS3PlayerControllerBase::OnClickedMainMenuButton()
{
	auto* UIManager = UUIDelegatesSubsystem::GetUIDelegateManager(GetWorld());
	UIManager->OnIsGameOver_UI.Broadcast(false);
	
	ServerRPC_OnClickedRestartGameButton();
	
}

void APS3PlayerControllerBase::ServerRPC_OnClickedMainMenuButton_Implementation()
{
	auto* PS3GameModeS5 = Cast<APS3GameModeS5>(GetWorld()->GetAuthGameMode());
	if (IsValid(PS3GameModeS5) == false) return;
	
	PS3GameModeS5->StageRestart();
}

void APS3PlayerControllerBase::ServerRPC_OnClickedRestartGameButton_Implementation()
{
	//TODO 타이틀레벨로 가는 로직 구현하기 (이때 호스트 게스트 연결 끊어짐?)
}


void APS3PlayerControllerBase::ConfigureInputMapping()
{
	if (IsLocalController() == false) return;
	
	GetWorld()->GetTimerManager().ClearTimer(InitTimerHandle);
	
	auto* HUD = Cast<APlayerHUD>(GetHUD());
	if (HUD == nullptr) return;

	PS3ViewModel = Cast<UPS3ViewModel>(HUD->GetViewModel());
	if (PS3ViewModel == nullptr) return;

	
	PS3ViewModel->OnGameRestartRequested_UI.AddDynamic(this, &ThisClass::OnClickedRestartGameButton);
	PS3ViewModel->OnExitToMainRequested_UI.AddDynamic(this, &ThisClass::OnClickedMainMenuButton);
	
	
	FInputModeUIOnly UIOnlyMode;
	SetInputMode(UIOnlyMode);

	bShowMouseCursor = true;
}
