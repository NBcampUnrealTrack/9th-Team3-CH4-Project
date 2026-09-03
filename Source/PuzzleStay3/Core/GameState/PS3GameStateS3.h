// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PS3GameStateBase.h"
#include "PS3GameStateS3.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStage3BlockingVolumeDisabled, bool, bDisabled);

UCLASS()
class PUZZLESTAY3_API APS3GameStateS3 : public APS3GameStateBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure)
	bool IsStage3BlockingVolumeDisabled() const { return bStage3BlockingVolumeDisabled; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetStage3BlockingVolumeDisabled(bool bDisabled);

	UPROPERTY(BlueprintAssignable)
	FOnStage3BlockingVolumeDisabled OnStage3BlockingVolumeDisabled;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Stage3BlockingVolumeDisabled)
	bool bStage3BlockingVolumeDisabled = false;

	UFUNCTION()
	void OnRep_Stage3BlockingVolumeDisabled();
};