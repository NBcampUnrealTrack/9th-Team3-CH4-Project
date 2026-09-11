// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PS3GamemodeBase.h"
#include "Data/Delegates/GameModeDelegates.h"
#include "Data/Enum/TimerUIType.h"
#include "PS3GameModeS5.generated.h"

enum class EPS3PlayerRole : uint8;
enum class EPS3TimerUIType: uint8;
class APS3PlayerCharacter;

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

	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	TObjectPtr<class US5_GameRuleDataAsset> S5_GameRuleDataAsset;
	
	
public:
	void ReSpawnPlayer(APlayerController* TargetPlayerController);
	void UnPossessedAndDestroyOldPawn(APlayerController* OldPlayerController);
	
	void OnTimerForGameStart();
	void OnGameStart();
	void OnGameOver();
	void OnQuitGame();
	
	void OnReduceGameTime();
	void OnTimeDeduction(float TimeToDeducted);

	

	void OnCollectLoginUser();
	int32 OnCollectGimmickBase();
	
public:
<<<<<<< HEAD
	
=======
	FOnIsGameStart OnIsGameStart;
	FOnScreenPlayerSpawned OnScreenPlayerSpawned;
	FOnIsInteractionGimmick OnIsInteractionGimmick;
>>>>>>> dev
	
	int32 RoleSelectedPlayerCount = 0;
	int32 MaxPlayerCount = 2;
	float WaitingTime = 3.0f;
	
	
	bool bIsAllPlayerSelectedRole = false;
	
	bool bIsTakeFieldControllerType = false;
	bool bIsTakeScreenControllerType = false;
	

	
protected:
	FTimerHandle AllPlayerReadyTimeHandle;
	FTimerHandle TimerForGameStartHandle;
	FTimerHandle GameLimitTimeHandle;

	int32 MaxEscapeDoorCount = 2;
	int32 MaxInteractionGimmickCount = 2;
	int32 TargetCountForSpawnScreenPlayer = 0;
	int32 ActivatedInteractionGimmickCount = 0;
	
	float ReducedTimeRange = 1.0f;
	
	bool bIsScreenPlayerSpawnReady = false;
	bool bIsScreenPlayerAlreadySpawned = false;
	
	
private:
	UPROPERTY()
	TArray<TObjectPtr<class APlayerController>> LoginUserArray;
	UPROPERTY()
	TArray<TObjectPtr<class AGimmickBase>> GimmickBaseArray;
	
	FTimerHandle InitTimerHandle;
	int32 InteractionGimmickCount = 0;
	bool bIsInteracted = false;
	
	void OnInteractedGimmick(bool bIsInteractedGimmick);
	virtual void InitializeToDataAssets() override;
	void InitializeGimmick();
	void RandomShuffleFakeGimmick();
	void BindInteractionGimmick();
	void ResistEscapeGimmick();
	void UnResistEscapeGimmick();
	
	
};

