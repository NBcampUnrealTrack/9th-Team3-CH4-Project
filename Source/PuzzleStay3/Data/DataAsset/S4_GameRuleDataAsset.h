// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/PS3StageType.h"
#include "Engine/DataAsset.h"
#include "S4_GameRuleDataAsset.generated.h"

class APS3PlayerCharacter;
enum class EPS3StageType : uint8;

UCLASS()
class PUZZLESTAY3_API US4_GameRuleDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	EPS3StageType StageType_S4 = EPS3StageType::Stage4;
	


};
