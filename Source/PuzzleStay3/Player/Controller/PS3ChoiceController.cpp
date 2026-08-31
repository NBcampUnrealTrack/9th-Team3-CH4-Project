// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3ChoiceController.h"

#include "Blueprint/UserWidget.h"
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

void APS3ChoiceController::ServerRPC_SelectedControllerType_Implementation(EPS3PlayerRoleType SelectedPlayerRoleType)
{
	if (PlayerState == nullptr) return;
	
	auto* PS3GameInstance = Cast<UPS3GameInstance>(GetGameInstance());
	if (PS3GameInstance == nullptr) return;
	
	const FUniqueNetIdRepl& PlayerID = PlayerState->GetUniqueId();
	PS3GameInstance->SetControllerEType(PlayerID, SelectedPlayerRoleType);
}

void APS3ChoiceController::OnClickedThirdPersonTypeButton()
{
	if (PlayerState == nullptr) return;
	
	auto* PS3GameInstance = Cast<UPS3GameInstance>(GetGameInstance());
	if (PS3GameInstance == nullptr) return;
	
	EPS3PlayerRoleType OnPlayerRoleType = EPS3PlayerRoleType::PlayerRole_ThirdPerson;
	EPS3PlayerRoleType CurrentPlayerRoleType = PS3GameInstance->GetControllerEType(PlayerState->GetUniqueId());
	
	if (CurrentPlayerRoleType == OnPlayerRoleType) return;
	ServerRPC_SelectedControllerType(OnPlayerRoleType);
	
	FString PlayerIdString = FString::FromInt(PlayerState->GetPlayerId());
	FString EnumString = UEnum::GetValueAsString(CurrentPlayerRoleType);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green,
		FString::Printf(TEXT("Player ID: %s | Current Role: %s"), *PlayerIdString, *EnumString));
}

void APS3ChoiceController::OnClickedScreenTypeButton()
{
	if (PlayerState == nullptr) return;
	
	auto* PS3GameInstance = Cast<UPS3GameInstance>(GetGameInstance());
	if (PS3GameInstance == nullptr) return;
	
	EPS3PlayerRoleType OnPlayerRoleType = EPS3PlayerRoleType::PlayerRole_Screen;
	EPS3PlayerRoleType CurrentPlayerRoleType = PS3GameInstance->GetControllerEType(PlayerState->GetUniqueId());
	
	if (CurrentPlayerRoleType == OnPlayerRoleType) return;
	ServerRPC_SelectedControllerType(OnPlayerRoleType);
	
	
	FString PlayerIdString = FString::FromInt(PlayerState->GetPlayerId());
	FString EnumString = UEnum::GetValueAsString(CurrentPlayerRoleType);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue,
		FString::Printf(TEXT("Player ID: %s | Current Role: %s"), *PlayerIdString, *EnumString));
}

