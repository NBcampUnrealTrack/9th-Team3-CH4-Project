#include "PS3PlayerController.h"

#include "Player/PlayerState/PS3PlayerState.h"

APS3PlayerController::APS3PlayerController()
{
	bShowMouseCursor = false;
}

void APS3PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = false;
	}
}

bool APS3PlayerController::IsFieldPlayer() const
{
	const APS3PlayerState* PS3PlayerState = GetPlayerState<APS3PlayerState>();
	return IsValid(PS3PlayerState) && PS3PlayerState->GetPlayerRole() == EPS3PlayerRole::Field;
}
