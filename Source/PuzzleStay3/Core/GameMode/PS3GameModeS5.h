// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PS3GamemodeBase.h"
#include "Data/Enum/PS3PlayerRoleType.h"
#include "PS3GameModeS5.generated.h"

class APS3PlayerCharacter;
enum class EPS3PlayerRole : uint8;
/**
 * 
 */
UCLASS()
class PUZZLESTAY3_API APS3GameModeS5 : public APS3GameModeBase
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;
	
public:
	void SetPlayerControllerRole(APlayerController* CurrentController, EPS3PlayerRole SelectedPlayerRoleType);
	void PossessedControllerAndSpawn(APlayerController* OldController, TSubclassOf<APlayerController> NewControllerClass, TSubclassOf<APS3PlayerCharacter> NewCharacterClass);
	void OnReduceGameTime();
	void OnGameStart();
	
	bool bIsTakeThirdPersonControllerType = false;
	bool bIsTakeScreenControllerType = false;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "GameRule|SwapController")
	TSubclassOf<APS3PlayerCharacter> ThirdPersonCharacterClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "GameRule|SwapController")
	TSubclassOf<APlayerController> ThirdPersonControllerClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "GameRule|SwapController")
	TSubclassOf<APS3PlayerCharacter> ScreenCharacterClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "GameRule|SwapController")
	TSubclassOf<APlayerController> ScreenControllerClass;

private:
	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	float GameLimitTime = 60.0f;
	
	FTimerHandle GameLimitTimeHandle;
	
	FString ThirdPersonString = "ThirdPersonPlayer";
	FString ScreenString = "ScreenPlayer" ;
	
	
};

