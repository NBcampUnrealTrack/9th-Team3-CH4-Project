// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PS3GameStateS5.generated.h"


/**
 * 
 */
UCLASS()
class PUZZLESTAY3_API APS3GameStateS5 : public AGameStateBase
{
	GENERATED_BODY()
	
protected:
	APS3GameStateS5();
	
	
protected:
	virtual void BeginPlay() override;
	
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
public:
	void OnGameOver();
	void OnReduceGameTime(float ReducedTimeRange);
	void OnTimeDeduction(float TimeToDeducted);
	
public:
	UPROPERTY(ReplicatedUsing = OnRep_GameLimitTime, EditAnywhere, Category = "GameRule")
	float GameLimitTime = 60.0f;
	
	

	UPROPERTY(ReplicatedUsing = OnRep_IsGameOver)
	bool bIsGameOver = false;

	
protected:
	UFUNCTION()
	void OnRep_IsGameOver();
	void SetIsGameOver_AuthorityOnRep(bool SetIsGameOver);
	
	UFUNCTION()
	void OnRep_GameLimitTime();
	void SetDeductGameLimitTime_AuthorityOnRep(float TimeToDeducted);


};
