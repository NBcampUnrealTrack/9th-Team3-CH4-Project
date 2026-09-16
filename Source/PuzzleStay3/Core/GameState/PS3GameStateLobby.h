// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PS3GameStateBase.h"
#include "Data/Enum/PS3TextNotifyType.h"
#include "PS3GameStateLobby.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLESTAY3_API APS3GameStateLobby : public APS3GameStateBase
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "PS3|Lobby")
	void ShowLobbyTextNotify(EPS3TextNotifyType NotifyType);

private:
	void ShowInitialLobbyTextNotify();
	bool TryShowLobbyTextNotify(EPS3TextNotifyType NotifyType) const;
	void RetryPendingLobbyTextNotify();

	FTimerHandle InitialLobbyTextNotifyTimerHandle;
	FTimerHandle LobbyTextNotifyRetryTimerHandle;
	EPS3TextNotifyType PendingLobbyTextNotify = EPS3TextNotifyType::None;
	int32 LobbyTextNotifyRetryCount = 0;

};
