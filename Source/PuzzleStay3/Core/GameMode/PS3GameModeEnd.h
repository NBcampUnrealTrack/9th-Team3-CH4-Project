// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PS3GamemodeBase.h"
#include "PS3GameModeEnd.generated.h"


UCLASS()
class PUZZLESTAY3_API APS3GameModeEnd : public APS3GameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	bool CanInteractFinalTreasure(APlayerState* PlayerState) const;
	bool RegisterFinalTreasureInteraction(APlayerState* PlayerState);
	
protected:
	virtual void InitializeToDataAssets() override;

	UPROPERTY(EditAnywhere, Category = "GameRule")
	TObjectPtr<class UEnd_GameRuleDataAsset> End_GameRuleDataAsset;
	
private:
	int32 RequiredPlayerCount;

	FString EndingVideoLevelPath;

	UPROPERTY()
	TArray<TObjectPtr<APlayerState>> InteractedPlayers;

	bool bEndingTriggered = false;

	void TravelToEndingVideoLevel();
};
