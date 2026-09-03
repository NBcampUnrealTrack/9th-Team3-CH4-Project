// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3ChoiceController.h"

#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Core/GameMode/PS3GameModeS5.h"
#include "Core/GameState/PS3GameStateS5.h"
#include "Data/Enum/PS3PlayerRole.h"


APS3ChoiceController::APS3ChoiceController()
{
	FixedCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FixedChoiceCamera"));
	SetRootComponent(FixedCameraComponent);
}

void APS3ChoiceController::BeginPlay()
{
	Super::BeginPlay();
	SetViewTarget(this);
	
	ConfigureInputMapping();
}

void APS3ChoiceController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	
	ConfigureInputMapping();
}

void APS3ChoiceController::UpdateRotation(float DeltaTime)
{
}

void APS3ChoiceController::ConfigureInputMapping()
{
	if (IsLocalController() == false) return;
	
	if (UIWidgetClass.IsValidIndex(0) == false) return;
	
	if (UIWidgetClass[0] == nullptr) return;
	
	UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass[0]);
	if (IsValid(UIWidgetInstance) == false) return;
	
	UIWidgetInstance->AddToViewport();

	FInputModeUIOnly UIOnlyMode;
	UIOnlyMode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());
	SetInputMode(UIOnlyMode);

	bShowMouseCursor = true;
	

}

void APS3ChoiceController::ServerRPC_SelectedControllerType_Implementation(EPS3PlayerRole SelectedPlayerRoleType)
{
	auto* PS3GameModeS5 = Cast<APS3GameModeS5>(GetWorld()->GetAuthGameMode());
	if (IsValid(PS3GameModeS5) == false) return;
	
	PS3GameModeS5->SetPlayerControllerRole(this, SelectedPlayerRoleType);
	
	if (PS3GameModeS5->RoleSelectedPlayerCount >= PS3GameModeS5->MaxPlayerCount)
	{
		PS3GameModeS5->OnGameStart();
	}
}

void APS3ChoiceController::OnClickedFieldTypeButton()
{
	
	if (bIsSelectedFieldType == true) return;
	bIsSelectedFieldType = true;
	
	ServerRPC_SelectedControllerType(EPS3PlayerRole::Field);
}

void APS3ChoiceController::OnClickedScreenTypeButton()
{
	if (bIsSelectedScreenType == true) return;
	bIsSelectedScreenType = true;
	
	ServerRPC_SelectedControllerType(EPS3PlayerRole::Screen);
}



