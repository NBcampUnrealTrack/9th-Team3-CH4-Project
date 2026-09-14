// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/PS3StageType.h"
#include "Data/Enum/TimerUIType.h"
#include "Engine/DataAsset.h"
#include "S5_GameRuleDataAsset.generated.h"

enum class EPS3StageType : uint8;
enum class EPS3TimerUIType : uint8;
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
	EPS3StageType StageType_S5 = EPS3StageType::Stage5;
	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	EPS3TimerUIType GameStartTimerUIType = EPS3TimerUIType::GameStartTimer;
	
	UPROPERTY(VisibleAnywhere, Category = "GameRule")
	int32 MaxPlayerCount = 2;
	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	int32 MaxInteractionGimmickCount = 2;
	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	int32 MaxEscapeDoorCount = 2;
	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	float ReducedTimeRange = 1.0f;
	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	float MaxGameLimitTime = 60.0f;
	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	float WaitingTime = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, Category = "SwapController")
	TSubclassOf<APS3PlayerCharacter> FieldCharacterClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "SwapController")
	TSubclassOf<APlayerController> FieldControllerClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "SwapController")
	TSubclassOf<APlayerController> ScreenControllerClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "SwapController")
	TSubclassOf<APlayerController> SpawnScreenControllerClass;

};
