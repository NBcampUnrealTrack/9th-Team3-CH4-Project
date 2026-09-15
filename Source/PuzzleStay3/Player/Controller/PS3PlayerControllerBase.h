// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PS3PlayerControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLESTAY3_API APS3PlayerControllerBase : public APlayerController
{
	GENERATED_BODY()
	
protected:
	
	virtual void BeginPlay() override;
	virtual void ReceivedPlayer() override;
	
protected:
	UPROPERTY()
	TObjectPtr<class UPS3ViewModel> PS3ViewModel;
	
	UFUNCTION()
	void OnClickedRestartGameButton();
	UFUNCTION()
	void OnClickedTitleMenuButton();
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_OnClickedRestartGameButton();
	UFUNCTION(Server, Reliable)
	void ServerRPC_OnClickedTitleButton();
	
public:
	void ConfigureInputMapping();
	
private:
	FTimerHandle InitTimerHandle;
	
};
