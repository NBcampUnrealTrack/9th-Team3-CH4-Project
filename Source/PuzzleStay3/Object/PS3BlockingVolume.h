// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/BlockingVolume.h"
#include "PS3BlockingVolume.generated.h"

UCLASS()
class PUZZLESTAY3_API APS3BlockingVolume : public ABlockingVolume
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void BlockingVolumeDisabled();
	
	FDelegateHandle BlockingVolumeDisabledHandle;
};
