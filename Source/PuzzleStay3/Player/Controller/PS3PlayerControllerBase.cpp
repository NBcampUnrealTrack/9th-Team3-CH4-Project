// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3PlayerControllerBase.h"

#include "Core/GameMode/PS3GamemodeBase.h"
#include "Core/GameMode/PS3GameModeS5.h"
#include "Core/GameState/PS3GameStateS5.h"
#include "Data/Delegates/UIDelegatesSubsystem.h"
#include "UI/HUD/PlayerHUD.h"
#include "UI/ViewModel/PS3ViewModel.h"


void APS3PlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();
	//GetWorld()->GetTimerManager().SetTimer(InitTimerHandle, this, &ThisClass::ConfigureInputMapping, 0.01f, false);
	
}


void APS3PlayerControllerBase::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	GetWorld()->GetTimerManager().SetTimer(InitTimerHandle, this, &ThisClass::ConfigureInputMapping, 0.01f, false);
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
	PS3ViewModel->OnExitToMainRequested_UI.AddDynamic(this, &ThisClass::OnClickedTitleMenuButton);
	
}


void APS3PlayerControllerBase::OnClickedRestartGameButton()
{
	ServerRPC_OnClickedRestartGameButton();
}


void APS3PlayerControllerBase::OnClickedTitleMenuButton()
{
	ServerRPC_OnClickedTitleButton();
}


void APS3PlayerControllerBase::ServerRPC_OnClickedRestartGameButton_Implementation()
{
	auto* PS3GameModeS5 = Cast<APS3GameModeS5>(GetWorld()->GetAuthGameMode());
	if (IsValid(PS3GameModeS5) == false) return;
	
	PS3GameModeS5->StageRestart();
}


void APS3PlayerControllerBase::ServerRPC_OnClickedTitleButton_Implementation()
{
	auto* PS3GameModeS5 = Cast<APS3GameModeS5>(GetWorld()->GetAuthGameMode());
	if (IsValid(PS3GameModeS5) == false) return;
	
	PS3GameModeS5->GotoTitleLevel();
}






