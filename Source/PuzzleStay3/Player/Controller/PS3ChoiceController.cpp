// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3ChoiceController.h"

#include "Camera/CameraComponent.h"
#include "Core/GameMode/PS3GameModeS5.h"
#include "Core/GameState/PS3GameStateS5.h"
#include "Data/Delegates/UIDelegatesSubsystem.h"
#include "Data/Enum/PS3PlayerRole.h"
#include "UI/HUD/PlayerHUD.h"
#include "UI/ViewModel/PS3ViewModel.h"


APS3ChoiceController::APS3ChoiceController()
{
	FixedCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FixedChoiceCamera"));
	SetRootComponent(FixedCameraComponent);
}


void APS3ChoiceController::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(InitTimerHandle, this, &ThisClass::ConfigureInputMapping, 0.1f, false);
	
	SetViewTarget(this);
}


void APS3ChoiceController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	
	ConfigureInputMapping();
}


void APS3ChoiceController::UpdateRotation(float DeltaTime)
{
	//고의로 아무것도 적지않고 부모의 내용을 차단하고 현 내용으로 함수 덮어씌기(오버라이드)
}


void APS3ChoiceController::ConfigureInputMapping()
{
	if (IsLocalController() == false) return;
	
	GetWorld()->GetTimerManager().ClearTimer(InitTimerHandle);
	
	auto* HUD = Cast<APlayerHUD>(GetHUD());
	if (HUD == nullptr) return;

	PS3ViewModel = Cast<UPS3ViewModel>(HUD->GetViewModel());
	if (PS3ViewModel == nullptr) return;

	PS3ViewModel->OnStage5RoleSelectionRequested_UI.AddDynamic(this, &ThisClass::OnClickedFieldTypeButton);
	PS3ViewModel->OnStage5RoleSelectionRequested_UI.AddDynamic(this, &ThisClass::OnClickedScreenTypeButton);
	
	PS3_BROADCAST_TO_MVVM_OneParams(OnRoleSelection_UI, true);
	
	FInputModeUIOnly UIOnlyMode;
	SetInputMode(UIOnlyMode);
	bShowMouseCursor = true;
}


void APS3ChoiceController::ServerRPC_SelectedControllerType_Implementation(EPS3PlayerRole SelectedPlayerRoleType)
{
	auto* PS3GameStateS5 = Cast<APS3GameStateS5>(GetWorld()->GetGameState());
	if (IsValid(PS3GameStateS5) == false) return;
	
	if (SelectedPlayerRoleType == EPS3PlayerRole::Field)
	{
		PS3GameStateS5->bIsSelectedFieldType = true;
	}
	
	else if (SelectedPlayerRoleType == EPS3PlayerRole::Screen)
	{
		PS3GameStateS5->bIsSelectedScreenType = true;
	}
	
	auto* PS3GameModeS5 = Cast<APS3GameModeS5>(GetWorld()->GetAuthGameMode());
	if (IsValid(PS3GameModeS5) == false) return;
	
	PS3GameModeS5->SetPlayerControllerRole(this, SelectedPlayerRoleType);
	
	if (PS3GameModeS5->RoleSelectedPlayerCount >= PS3GameModeS5->MaxPlayerCount)
	{
		PS3GameModeS5->OnTimerForGameStart();
	}
}


void APS3ChoiceController::OnClickedFieldTypeButton(EPS3PlayerRole SelectType)
{
	if (SelectType == EPS3PlayerRole::Field)
	{
		auto* PS3GameStateS5 = Cast<APS3GameStateS5>(GetWorld()->GetGameState());
		if (IsValid(PS3GameStateS5) == false) return;
		
		if (PS3GameStateS5->bIsSelectedFieldType == true) return;
	
		ServerRPC_SelectedControllerType(SelectType);
		
		PS3_BROADCAST_TO_MVVM_OneParams(OnRoleSelection_UI, false);
		
	}
}


void APS3ChoiceController::OnClickedScreenTypeButton(EPS3PlayerRole SelectType)
{
	if (SelectType == EPS3PlayerRole::Screen)
	{
		auto* PS3GameStateS5 = Cast<APS3GameStateS5>(GetWorld()->GetGameState());
		if (IsValid(PS3GameStateS5) == false) return;
		
		if (PS3GameStateS5->bIsSelectedScreenType == true) return;
	
		ServerRPC_SelectedControllerType(SelectType);
		
		PS3_BROADCAST_TO_MVVM_OneParams(OnRoleSelection_UI, false);
		
	}
}



