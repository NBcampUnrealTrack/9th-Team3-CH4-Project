// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameStateLobby.h"

#include "Engine/GameInstance.h"
#include "Player/Online/PS3LobbySubsystem.h"
#include "UI/HUD/PlayerHUD.h"

namespace
{
	constexpr float LobbyTextNotifyRetryInterval = 0.2f;
	constexpr int32 MaxLobbyTextNotifyRetryCount = 25;
}

void APS3GameStateLobby::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(
		InitialLobbyTextNotifyTimerHandle,
		this,
		&ThisClass::ShowInitialLobbyTextNotify,
		2.0,
		false);
}

void APS3GameStateLobby::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(InitialLobbyTextNotifyTimerHandle);
	GetWorldTimerManager().ClearTimer(LobbyTextNotifyRetryTimerHandle);

	Super::EndPlay(EndPlayReason);
}

void APS3GameStateLobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void APS3GameStateLobby::ShowInitialLobbyTextNotify()
{
	EPS3TextNotifyType NotifyType = EPS3TextNotifyType::LobbyFirst;

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UPS3LobbySubsystem* LobbySubsystem = GameInstance->GetSubsystem<UPS3LobbySubsystem>())
		{
			const EPS3TextNotifyType PendingNotifyType = LobbySubsystem->ConsumePendingTextNotify();
			if (PendingNotifyType != EPS3TextNotifyType::None)
			{
				NotifyType = PendingNotifyType;
			}
		}
	}

	ShowLobbyTextNotify(NotifyType);
}

void APS3GameStateLobby::ShowLobbyTextNotify(EPS3TextNotifyType NotifyType)
{
	if (NotifyType == EPS3TextNotifyType::None)
	{
		return;
	}

	if (TryShowLobbyTextNotify(NotifyType))
	{
		PendingLobbyTextNotify = EPS3TextNotifyType::None;
		LobbyTextNotifyRetryCount = 0;
		GetWorldTimerManager().ClearTimer(LobbyTextNotifyRetryTimerHandle);
		return;
	}

	PendingLobbyTextNotify = NotifyType;
	LobbyTextNotifyRetryCount = 0;

	GetWorldTimerManager().SetTimer(
		LobbyTextNotifyRetryTimerHandle,
		this,
		&ThisClass::RetryPendingLobbyTextNotify,
		LobbyTextNotifyRetryInterval,
		true);
}

bool APS3GameStateLobby::TryShowLobbyTextNotify(EPS3TextNotifyType NotifyType) const
{
	if (!GetWorld())
	{
		return false;
	}

	bool bShown = false;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PlayerController = It->Get();
		if (!IsValid(PlayerController) || !PlayerController->IsLocalController())
		{
			continue;
		}

		APlayerHUD* PlayerHUD = Cast<APlayerHUD>(PlayerController->GetHUD());
		if (!IsValid(PlayerHUD) || !PlayerHUD->IsUIReady())
		{
			continue;
		}

		PlayerHUD->SetTextNotifyVisible(true);
		PlayerHUD->ShowTextNotify(NotifyType);
		bShown = true;
	}

	return bShown;
}

void APS3GameStateLobby::RetryPendingLobbyTextNotify()
{
	if (PendingLobbyTextNotify == EPS3TextNotifyType::None)
	{
		GetWorldTimerManager().ClearTimer(LobbyTextNotifyRetryTimerHandle);
		return;
	}

	if (TryShowLobbyTextNotify(PendingLobbyTextNotify))
	{
		PendingLobbyTextNotify = EPS3TextNotifyType::None;
		LobbyTextNotifyRetryCount = 0;
		GetWorldTimerManager().ClearTimer(LobbyTextNotifyRetryTimerHandle);
		return;
	}

	++LobbyTextNotifyRetryCount;
	if (LobbyTextNotifyRetryCount >= MaxLobbyTextNotifyRetryCount)
	{
		GetWorldTimerManager().ClearTimer(LobbyTextNotifyRetryTimerHandle);
	}
}
