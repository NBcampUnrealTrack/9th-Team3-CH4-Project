// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PS3GameStateBase.h"
#include "PS3GameStateS4.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStage4WeightChanged, float, FixedWeight, float, TargetWeight);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStage4FirstDoorOpenedChanged, bool, bOpened);

UCLASS()
class PUZZLESTAY3_API APS3GameStateS4 : public APS3GameStateBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void InitializeToDataAssets() override;

	UPROPERTY(EditAnywhere, Category = "GameRule")
	TObjectPtr<class US4_GameRuleDataAsset> S4_GameRuleDataAsset;

public:
	UFUNCTION(BlueprintPure)
	float GetFixedObjectWeight() const { return FixedObjectWeight; }

	UFUNCTION(BlueprintPure)
	float GetTargetBalancedWeight() const { return TargetBalancedWeight; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetStage4Weights(float NewFixedWeight, float NewTargetWeight);

	UPROPERTY(BlueprintAssignable)
	FOnStage4WeightChanged OnStage4WeightChanged;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Stage4Weights)
	float FixedObjectWeight = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_Stage4Weights)
	float TargetBalancedWeight = 0.f;

	UFUNCTION()
	void OnRep_Stage4Weights();

public:
	UPROPERTY(BlueprintAssignable)
	FOnStage4FirstDoorOpenedChanged OnStage4FirstDoorOpenedChanged;

	UFUNCTION(BlueprintPure)
	bool IsStage4FirstDoorOpened() const { return bStage4FirstDoorOpened; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetStage4FirstDoorOpened(bool bOpened);

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Stage4FirstDoorOpened)
	bool bStage4FirstDoorOpened = false;

	UFUNCTION()
	void OnRep_Stage4FirstDoorOpened();
};
