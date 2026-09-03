
#pragma once

#include "CoreMinimal.h"
#include "PS3GameStateBase.h"
#include "PS3GameStateS5.generated.h"


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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
public:
	void OnGameOver();
	void OnReduceGameTime(float ReducedTimeRange);
	void OnTimeDeduction(float TimeToDeducted);
	
	//TODO 테스트용 BlueprintCallable 임. 나중에 삭제 할 것
	UFUNCTION(BlueprintCallable)
	void StageRestart();
	UFUNCTION(BlueprintCallable)
	void OnQuitGame();
	
public:
	UPROPERTY(Replicated)
	bool bIsSelectedFieldType = false;
	UPROPERTY(Replicated)
	bool bIsSelectedScreenType = false;
	
	UPROPERTY(ReplicatedUsing = OnRep_GameLimitTime, EditAnywhere, Category = "GameRule")
	float GameLimitTime = 60.0f;
	
	UPROPERTY(ReplicatedUsing = OnRep_IsGameOver)
	bool bIsGameOver = false;

protected:
	UFUNCTION()
	void OnRep_IsGameOver();
	void SetIsGameOver_AuthorityOnRep(bool SetIsGameOver);
	
	UFUNCTION()
	void OnRep_GameLimitTime();
	void SetDeductGameLimitTime_AuthorityOnRep(float TimeToDeducted);
	
};
