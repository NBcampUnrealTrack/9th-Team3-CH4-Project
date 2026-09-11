// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/PS3StageType.h"
#include "Data/Enum/PS3TextNotifyType.h"
#include "Engine/DataAsset.h"
#include "S2_GameRuleDataAsset.generated.h"

class APS3PlayerCharacter;
enum class EPS3StageType : uint8;

UCLASS()
class PUZZLESTAY3_API US2_GameRuleDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	EPS3StageType StageType_S2 = EPS3StageType::Stage2;
	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	EPS3TextNotifyType TextNotifyTypeForUI_S2 = EPS3TextNotifyType::Stage2;
	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	TSoftObjectPtr<UWorld> NextStageLevel;
};
