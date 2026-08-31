#include "PS3ScreenPlayerController.h"

#include "Player/PlayerState/PS3PlayerState.h"

APS3ScreenPlayerController::APS3ScreenPlayerController()
{
	bShowMouseCursor = true;
}

void APS3ScreenPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		SetIgnoreMoveInput(true);
		SetIgnoreLookInput(true);

		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);
		bShowMouseCursor = true;
	}
}

bool APS3ScreenPlayerController::IsScreenPlayer() const
{
	const APS3PlayerState* PS3PlayerState = GetPlayerState<APS3PlayerState>();
	return IsValid(PS3PlayerState) && PS3PlayerState->GetPlayerRole() == EPS3PlayerRole::Screen;
}
