#include "PS3ScreenPlayerController.h"

#include "Player/PlayerState/PS3PlayerState.h"
#include "UI/HUD/PlayerHUD.h"

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
		RefreshPlayerHUD();
	}
}

void APS3ScreenPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	RefreshPlayerHUD();
}

bool APS3ScreenPlayerController::IsScreenPlayer() const
{
	const APS3PlayerState* PS3PlayerState = GetPlayerState<APS3PlayerState>();
	return IsValid(PS3PlayerState) && PS3PlayerState->GetPlayerRole() == EPS3PlayerRole::Screen;
}

void APS3ScreenPlayerController::RefreshPlayerHUD() const
{
	if (APlayerHUD* PlayerHUD = Cast<APlayerHUD>(GetHUD()))
	{
		PlayerHUD->InitializeFromPlayerState(GetPlayerState<APS3PlayerState>());
	}
}
