// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/BlockingVolume.h"
#include "PS3BlockingVolume.generated.h"

class UBoxComponent;

UCLASS()
class PUZZLESTAY3_API APS3BlockingVolume : public AActor
{
	GENERATED_BODY()
	
	public:
	APS3BlockingVolume();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void BlockingVolumeDisabled();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	TObjectPtr<UBoxComponent> BoxCompo;
	
	FDelegateHandle BlockingVolumeDisabledHandle;
};
