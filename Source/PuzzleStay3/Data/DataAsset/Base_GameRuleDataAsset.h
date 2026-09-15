// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Base_GameRuleDataAsset.generated.h"

enum class EPS3StageType : uint8;

UCLASS()
class PUZZLESTAY3_API UBase_GameRuleDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:	
	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	float StageClearDelay = 10.0f;
	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	float StageType_UI_DelayTime = 0.1f;
	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	float TextNotify_UI_DelayTime = 0.5f;
	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	TSoftObjectPtr<UWorld> TitleLevel;

};
