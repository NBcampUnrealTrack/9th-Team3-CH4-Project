// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PS3GamemodeBase.h"
#include "PS3GameModeS5.generated.h"

class APS3PlayerCharacter;
enum class EPS3PlayerRole : uint8;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsGameStart, bool)
DECLARE_MULTICAST_DELEGATE(FOnScreenPlayerSpawned)
DECLARE_MULTICAST_DELEGATE(FOnStageClear)
UCLASS()
class PUZZLESTAY3_API APS3GameModeS5 : public APS3GameModeBase
{
	GENERATED_BODY()
	
	APS3GameModeS5();
	
public:
	virtual void PostLogin(APlayerController* NewPlayer);
	virtual void BeginPlay() override;
	virtual void StageRestart() override;
	

#pragma region ConfigureControllerAndSpawn
	
	//스폰관련 함수
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName = L"") override;
	void SetPlayerControllerRole(APlayerController* CurrentController, EPS3PlayerRole SelectedPlayerRoleType);
	void ConfigureControllerAndSpawn(APlayerController* OldController, TSubclassOf<APlayerController> NewControllerClass);
	
#pragma endregion
	
	
protected:
	UPROPERTY()
	TArray<TObjectPtr<class APlayerController>> LoginUserArray;
	UPROPERTY()
	TArray<TObjectPtr<class AGimmickBase>> GimmickBaseArray;
	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	TObjectPtr<class US5_GameRuleDataAsset> S5_GameRuleDataAsset;
	
	
public:
	void ReSpawnPlayer(APlayerController* TargetPlayerController);
	void UnPossessedAndDestroyOldPawn(APlayerController* OldPlayerController);
	
	void OnGameStart();
	void OnGameOver();
	void OnQuitGame();
	
	void OnReduceGameTime();
	void OnTimeDeduction(float TimeToDeducted);

	
	void OnInteractedEscapeDoor(bool bIsInteracted);
	void RandomInitializeEscapeDoor();
	void OnCollectLoginUser();
	int32 OnCollectEscapeDoor();
	
public:
	FOnIsGameStart OnIsGameStart;
	FOnScreenPlayerSpawned OnScreenPlayerSpawned;
	
	int32 RoleSelectedPlayerCount = 0;
	int32 MaxPlayerCount = 2;
	
	bool bIsAllPlayerSelectedRole = false;
	
	bool bIsTakeFieldControllerType = false;
	bool bIsTakeScreenControllerType = false;
	
	
protected:
	FTimerHandle AllPlayerReadyTimeHandle;
	FTimerHandle GameLimitTimeHandle;

	int32 GoalEscapeDoorCount = 0;
	int32 ActivatedEscapeDoorCount = 0;
	
	bool bIsScreenPlayerSpawnReady = false;
	bool bIsScreenPlayerSpawnedField = false;

};

