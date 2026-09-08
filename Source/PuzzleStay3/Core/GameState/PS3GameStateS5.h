
#pragma once

#include "CoreMinimal.h"
#include "PS3GameStateBase.h"
#include "PS3GameStateS5.generated.h"


class APS3GameModeS5;

/**
 * 
 */
UCLASS()
class PUZZLESTAY3_API APS3GameStateS5 : public APS3GameStateBase
{
	GENERATED_BODY()
	
protected:
	APS3GameStateS5();
	
	
public:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
public:
	void ReSpawnPlayer(APlayerController* TargetPlayerController);
	
	void StageRestart();
	void OnQuitGame();
	
	void OnGameOver();
	void OnReduceGameTime(float ReducedTimeRange);
	void OnTimeDeduction(float TimeToDeducted);
	
	
public:
	UPROPERTY(EditAnywhere, Category = "GameRule")
	TObjectPtr<class US5_GameRuleDataAsset> S5_GameRuleDataAsset;
	
	UPROPERTY(ReplicatedUsing = OnRep_GameLimitTime)
	float GameLimitTime = 60.0f;
	
	UPROPERTY(Replicated)
	bool bIsSelectedFieldType = false;
	UPROPERTY(Replicated)
	bool bIsSelectedScreenType = false;
	
	UPROPERTY(ReplicatedUsing = OnRep_IsGameOver)
	bool bIsGameOver = false;
	
	UPROPERTY()
	TObjectPtr<APS3GameModeS5> CastPS3GameModeS5;
	
protected:
	UFUNCTION()
	void OnRep_IsGameOver();
	void SetIsGameOver_AuthorityOnRep(bool SetIsGameOver);
	
	UFUNCTION()
	void OnRep_GameLimitTime();
	void SetDeductGameLimitTime_AuthorityOnRep(float TimeToDeducted);
	
	APS3GameModeS5* GetCastPS3GameModeS5();
};
