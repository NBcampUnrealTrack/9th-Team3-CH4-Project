// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PS3GamemodeBase.h"
#include "PS3GameModeS5.generated.h"

class APS3PlayerCharacter;
enum class EPS3PlayerRole : uint8;

/*TODO 
 스테이지5에서는 캐릭터의 스타팅 지점이 둘로 나뉩니다.
 그래서 스타팅지점의 액터에 Tag가 붙어있습니다.
 해당 태그의 내용은 아래의 private 변수의 string 내용과 동일하게 존재해야합니다.
 이를 언리얼 에디터에서 꼭 스타트포인트 액터의 Tag에 작성해주세요.
 (사실 다른 스테이지도 마찬가지일테죠)
 
 마찬가지로 GimmickBase도 Tag를 에디터에서 작성해서
 Escape 기믹을 따로 구분해야합니다.*/
 
DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsGameStart, bool)
UCLASS()
class PUZZLESTAY3_API APS3GameModeS5 : public APS3GameModeBase
{
	GENERATED_BODY()
	
	APS3GameModeS5();
	
public:
	virtual void PostLogin(APlayerController* NewPlayer);
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual void BeginPlay() override;
	virtual void StageRestart() override;

	
protected:
	UPROPERTY(EditAnywhere, Category = "GameRule")
	TObjectPtr<class US5_GameRuleDataAsset> S5_GameRuleDataAsset;
	
	UPROPERTY()
	TArray<TObjectPtr<class UInteractionSwitchComponent>> EscapeGimmickArray;
	
	UPROPERTY()
	TArray<TObjectPtr<class APlayerController>> LoginUserArray;
	
public:
	void SetPlayerControllerRole(APlayerController* CurrentController, EPS3PlayerRole SelectedPlayerRoleType);
	void ConfigureControllerAndSpawn(
		APlayerController* OldController, 
		TSubclassOf<APlayerController> NewControllerClass, 
		TSubclassOf<APS3PlayerCharacter> NewCharacterClass);
	
	void OnGameStart();
	void OnGameOver();
	
	void OnQuitGame();
	
	void OnReduceGameTime();
	void OnTimeDeduction(float TimeToDeducted);
	
	void EscapeGimmickDetection();
	void OnEscapeGimmickUnlocked();
	
	void OnCollectLoginUser();


public:
	FOnIsGameStart OnIsGameStart;
	
	int32 RoleSelectedPlayerCount = 0;
	int32 MaxPlayerCount = 2;
	
	bool bIsAllPlayerSelectedRole = false;
	
	bool bIsTakeFieldControllerType = false;
	bool bIsTakeScreenControllerType = false;
	
	
	
	
protected:
	void FieldPlayerConfigureAndSpawn(
		APlayerController* OldController, 
	TSubclassOf<APlayerController> NewControllerClass, 
	TSubclassOf<APS3PlayerCharacter> NewCharacterClass,
	FString TargetTag);
	
	void ScreenPlayerConfigure(
		APlayerController* OldController, 
		TSubclassOf<APlayerController> NewControllerClass);
	
	void ScreenPlayerSpawnCharacter(APlayerController* OldController, 
	TSubclassOf<APlayerController> NewControllerClass, 
	TSubclassOf<APS3PlayerCharacter> NewCharacterClass,
	FString TargetTag);
	
protected:
	FTimerHandle AllPlayerReadyTimeHandle;
	FTimerHandle GameLimitTimeHandle;

	
private:
	FName EscapeGimmickTagName = "EscapeGimmick" ;
	

};

