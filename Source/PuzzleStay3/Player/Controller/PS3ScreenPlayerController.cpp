#include "PS3ScreenPlayerController.h"

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
	return true;
}
