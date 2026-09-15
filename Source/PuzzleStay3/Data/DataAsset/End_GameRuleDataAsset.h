// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "End_GameRuleDataAsset.generated.h"

enum class EPS3StageType : uint8;

UCLASS()
class PUZZLESTAY3_API UEnd_GameRuleDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:	
	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	int32 RequiredPlayerCount = 2;

	UPROPERTY(EditAnywhere, Category = "GameRule")
	TSoftObjectPtr<UWorld> EndingVideoLevel;
};
