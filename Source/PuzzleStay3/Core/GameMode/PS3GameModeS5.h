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
 이를 언리얼 에디터에서 꼭 스타트포인트 액터의 Tag에 작성해주세요
 (사실 다른 스테이지도 마찬가지일테죠)*/
 
DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsGameStart, bool)
UCLASS()
class PUZZLESTAY3_API APS3GameModeS5 : public APS3GameModeBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
public:
	void SetPlayerControllerRole(APlayerController* CurrentController, EPS3PlayerRole SelectedPlayerRoleType);
	void PossessedControllerAndSpawn(
		APlayerController* OldController, 
		TSubclassOf<APlayerController> NewControllerClass, 
		TSubclassOf<APS3PlayerCharacter> NewCharacterClass);
	
	void OnGameStart();
	void OnGameOver();
	void OnReduceGameTime();
	void OnTimeDeduction(float TimeToDeducted);
	
	//virtual void StageRestart() override;
	
	int32 RoleSelectedPlayerCount = 0;
	int32 MaxPlayerCount = 2;
	bool bIsAllPlayerSelectedRole = false;
	
	bool bIsTakeFieldControllerType = false;
	bool bIsTakeScreenControllerType = false;
	
	FOnIsGameStart OnIsGameStart;
	
public:
	UPROPERTY(EditAnywhere, Category = "GameRule")
	float ReducedTimeRange = 1.0f;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "GameRule|SwapController")
	TSubclassOf<APS3PlayerCharacter> FieldCharacterClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "GameRule|SwapController")
	TSubclassOf<APlayerController> FieldControllerClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "GameRule|SwapController")
	TSubclassOf<APS3PlayerCharacter> ScreenCharacterClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "GameRule|SwapController")
	TSubclassOf<APlayerController> ScreenControllerClass;
	
protected:
	void ConfigureSpawnFieldControllerPlayer(
		APlayerController* OldController, 
	TSubclassOf<APlayerController> NewControllerClass, 
	TSubclassOf<APS3PlayerCharacter> NewCharacterClass,
	FString TargetTag);
	
	void ConfigureScreenControllerPlayer(
		APlayerController* OldController, 
		TSubclassOf<APlayerController> NewControllerClass);
	
	void SpawnScreenControllerPlayer(APlayerController* OldController, 
	TSubclassOf<APlayerController> NewControllerClass, 
	TSubclassOf<APS3PlayerCharacter> NewCharacterClass,
	FString TargetTag);
	
protected:
	FTimerHandle AllPlayerReadyTimeHandle;
	FTimerHandle GameLimitTimeHandle;

	
private:
	FString FieldPlayerTagString = "FieldPlayer";
	FString ScreenPlayerTagString = "ScreenPlayer" ;
};

