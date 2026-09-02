// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PS3GameState.generated.h"


/**
 * 
 */
UCLASS()
class PUZZLESTAY3_API APS3GameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated, EditAnywhere, Category = "GameRule")
	float GameLimitTime = 60.0f;

};
