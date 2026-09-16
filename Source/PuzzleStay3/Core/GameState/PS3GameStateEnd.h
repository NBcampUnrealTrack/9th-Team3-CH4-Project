// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PS3GameStateBase.h"
#include "PS3GameStateEnd.generated.h"

UCLASS()
class PUZZLESTAY3_API APS3GameStateEnd : public APS3GameStateBase
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void InitializeToDataAssets() override;
	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	TObjectPtr<class UEnd_GameRuleDataAsset> End_GameRuleDataAsset;
};

