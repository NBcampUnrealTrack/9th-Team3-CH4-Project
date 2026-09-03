// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PS3GameStateBase.h"
#include "PS3GameStateS1.generated.h"
//DECLARE_MULTICAST_DELEGATE_OneParam(FOnBlockingVolumeDisabled, EPS3StageNumber);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStage1BlockingVolumeDisabled, bool, bDisabled);

UCLASS()
class PUZZLESTAY3_API APS3GameStateS1 : public APS3GameStateBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure)
	bool IsStage1BlockingVolumeDisabled() const { return bStage1BlockingVolumeDisabled; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetStage1BlockingVolumeDisabled(bool bDisabled);

	UPROPERTY(BlueprintAssignable)
	FOnStage1BlockingVolumeDisabled OnStage1BlockingVolumeDisabled;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Stage1BlockingVolumeDisabled)
	bool bStage1BlockingVolumeDisabled = false;

	UFUNCTION()
	void OnRep_Stage1BlockingVolumeDisabled();
};
