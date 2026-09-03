// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Stage5ControllerDataAsset.generated.h"

class APS3PlayerCharacter;
/**
 * 
 */
UCLASS()
class PUZZLESTAY3_API UStage5ControllerDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "SwapController")
	TSubclassOf<APS3PlayerCharacter> FieldCharacterClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "SwapController")
	TSubclassOf<APlayerController> FieldControllerClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "SwapController")
	TSubclassOf<APS3PlayerCharacter> ScreenCharacterClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "SwapController")
	TSubclassOf<APlayerController> ScreenControllerClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "SwapTag")
	FString FieldPlayerTagString = "FieldPlayer";
	
	UPROPERTY(EditDefaultsOnly, Category = "SwapTag")
	FString ScreenPlayerTagString = "ScreenPlayer" ;
};
