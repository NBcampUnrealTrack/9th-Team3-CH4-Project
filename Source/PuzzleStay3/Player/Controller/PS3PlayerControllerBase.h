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
	
	void OnClickedRestartGameButton();
	void OnClickedMainMenuButton();
};
