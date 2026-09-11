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
	
protected:
	UPROPERTY()
	TObjectPtr<class UPS3ViewModel> PS3ViewModel;
	
	void ConfigureInputMapping();
	
	UFUNCTION()
	void OnClickedRestartGameButton();
	UFUNCTION()
	void OnClickedMainMenuButton();
	
	UFUNCTION(Server, Unreliable)
	void ServerRPC_OnClickedRestartGameButton();
	UFUNCTION(Server, Unreliable)
	void ServerRPC_OnClickedMainMenuButton();
	
public:
	
	
private:
	FTimerHandle InitTimerHandle;
	
};
