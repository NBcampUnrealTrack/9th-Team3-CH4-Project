// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PS3GameModeBase.h"
#include "PS3GameModeS4.generated.h"


UCLASS()
class PUZZLESTAY3_API APS3GameModeS4 : public APS3GameModeBase
{
	GENERATED_BODY()

	/*
	저울고정 = 500, 520, 550
	P1 = 100
	P2 = 70
	
	덤벨1 = 200
	덤벨2 = 180
	덤벨3 = 150
	 
	case1 = 500 - 170 = 330 = 덤벨2 (180) + 덤벨3 (150)
	case3 = 520 - 170 = 350 = 덤벨1 (200) + 덤벨3 (150)
 	case2 = 550 - 170 = 380 = 덤벨1 (200) + 덤벨2 (180)
	*/
	
	//저울 담당이 코드에 반영하도록 전달하기
	//Door에 타입을 심어줘야할듯? StageAllFinalDoor, Stage1NormalDoor, Stage4FirstDoor, Stage5NormalDoor 
	//Door class에 컴포넌트 형식으로 각 스테이지에 배치될 door의 내용을 넣어줘도 되고... 맘대로
	
public:
	virtual void BeginPlay() override;

protected:
	virtual void InitializeToDataAssets() override;
	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	TObjectPtr<class US4_GameRuleDataAsset> S4_GameRuleDataAsset;
	
public:
	UFUNCTION(BlueprintPure)
	float GetTargetBalancedWeight() const { return TargetBalancedWeight; }

	UFUNCTION(BlueprintPure)
	float GetFixedObjectWeight() const { return FixedObjectWeight; }

	void NotifyJeoulResult(bool bIsSuccess);
	
	void OpenStage4FirstDoor();

private:
	TArray<float> SubstituteFixedObjectWeight;

	float Player1Weight;

	float Player2Weight;

	float FixedObjectWeight = 0.f;
	float TargetBalancedWeight = 0.f;
};

