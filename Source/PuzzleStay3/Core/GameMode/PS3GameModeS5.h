// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PS3GamemodeBase.h"
#include "PS3GameModeS5.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLESTAY3_API APS3GameModeS5 : public APS3GameModeBase
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;
	
public:
	
	void OnReduceGameTime();
	
	
private:
	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	float GameLimitTime = 60.0f;
	
	
	FTimerHandle GameLimitTimeHandle;
	
};
