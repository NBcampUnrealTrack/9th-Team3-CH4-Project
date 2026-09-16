// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/PS3PlayerIdentity.h"
#include "Engine/GameInstance.h"
#include "PS3GameInstance.generated.h"

UCLASS()
class PUZZLESTAY3_API UPS3GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	bool TryGetCachedPlayerIdentity(const FString& PlayerKey, EPS3PlayerIdentity& OutIdentity) const;
	void CachePlayerIdentity(const FString& PlayerKey, EPS3PlayerIdentity Identity);

private:
	UPROPERTY()
	TMap<FString, EPS3PlayerIdentity> PlayerIdentityByKey;
};