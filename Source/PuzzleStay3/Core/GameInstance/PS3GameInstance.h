// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PuzzleStay3/Data/Enum/PS3PlayerRoleType.h"
#include "PS3GameInstance.generated.h"

enum class EPS3PlayerRoleType : uint8;
/**
 * 
 */
UCLASS()
class PUZZLESTAY3_API UPS3GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	
	
	TMap<FUniqueNetIdRepl, EPS3PlayerRoleType> ControllerTypesMap;
	
	void SetControllerEType(const FUniqueNetIdRepl& SetPlayerID, EPS3PlayerRoleType SetControllerType);
	EPS3PlayerRoleType GetControllerEType(const FUniqueNetIdRepl& FindPlayerID);	
	
};
