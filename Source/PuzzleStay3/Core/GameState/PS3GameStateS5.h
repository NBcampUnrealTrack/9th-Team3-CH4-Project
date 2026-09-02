// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PS3GameStateBase.generated.h"


/**
 * 
 */
UCLASS()
class PUZZLESTAY3_API APS3GameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	
protected:
	virtual void BeginPlay() override;
	
	
public:
	void OnRep_IsGameOver();
	
	
protected:
	void OnGameStart();
	void OnGameOver();
	void OnReduceGameTime();
	
public:
	UPROPERTY(Replicated, EditAnywhere, Category = "GameRule")
	float GameLimitTime = 60.0f;
	
	UPROPERTY(ReplicatedUsing = OnRep_IsGameOver)
	bool bIsGameOver = false;
	
	
protected:
	FTimerHandle GameLimitTimeHandle;
	
	


};
