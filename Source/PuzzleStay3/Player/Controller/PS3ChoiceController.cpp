// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3ChoiceController.h"

#include "Blueprint/UserWidget.h"
#include "Core/GameMode/PS3GameModeS5.h"
#include "Core/GameState/PS3GameState.h"
#include "Data/Enum/PS3PlayerRole.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "PuzzleStay3/Core/GameInstance/PS3GameInstance.h"



void APS3ChoiceController::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocalController() == false) return;
	
	if (UIWidgetClass.IsValidIndex(0) == false) return;
	
	if (UIWidgetClass[0] == nullptr) return;
	
	UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass[0]);
	if (IsValid(UIWidgetInstance) == false) return;
		
	UIWidgetInstance->AddToViewport();

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());
	SetInputMode(Mode);

	bShowMouseCursor = true;
}

void APS3ChoiceController::ServerRPC_SelectedControllerType_Implementation(EPS3PlayerRole SelectedPlayerRoleType)
{
	auto* PS3GameModeS5 = Cast<APS3GameModeS5>(GetWorld()->GetAuthGameMode());
	if (IsValid(PS3GameModeS5) == false) return;
	
	PS3GameModeS5->SetPlayerControllerRole(this, SelectedPlayerRoleType);
	
}

void APS3ChoiceController::OnClickedThirdPersonTypeButton()
{
	if (bIsSelectedThirdPersonType == true) return;
	bIsSelectedThirdPersonType = true;
	
	ServerRPC_SelectedControllerType(EPS3PlayerRole::Field);
}

void APS3ChoiceController::OnClickedScreenTypeButton()
{
	if (bIsSelectedScreenType == true) return;
	bIsSelectedScreenType = true;
	
	ServerRPC_SelectedControllerType(EPS3PlayerRole::Screen);
}

