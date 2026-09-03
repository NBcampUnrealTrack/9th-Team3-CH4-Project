#include "PS3ScreenPlayerController.h"

APS3ScreenPlayerController::APS3ScreenPlayerController()
{
	bShowMouseCursor = true;
}

void APS3ScreenPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ConfigureLocalInputMode();
}

void APS3ScreenPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	ConfigureLocalInputMode();
}

void APS3ScreenPlayerController::ConfigureLocalInputMode()
{
	if (!bLocalInputModeConfigured && IsLocalController())
	{
		SetIgnoreMoveInput(true);
		SetIgnoreLookInput(true);

		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);
		bShowMouseCursor = true;
		bLocalInputModeConfigured = true;
	}
}

bool APS3ScreenPlayerController::IsScreenPlayer() const
{
	return true;
}
