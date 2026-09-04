// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "S5_GameRuleDataAsset.generated.h"

class APS3PlayerCharacter;
/**
 * 
 */
UCLASS()
class PUZZLESTAY3_API US5_GameRuleDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:	
	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	float ReducedTimeRange = 1.0f;
	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	float MaxGameLimitTime = 60.0f;
	
	
	UPROPERTY(EditDefaultsOnly, Category = "SwapController")
	TSubclassOf<APS3PlayerCharacter> FieldCharacterClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "SwapController")
	TSubclassOf<APlayerController> FieldControllerClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "SwapController")
	TSubclassOf<APlayerController> ScreenControllerClass;

};
