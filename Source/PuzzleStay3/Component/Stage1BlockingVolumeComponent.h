// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Stage1BlockingVolumeComponent.generated.h"

//모드한테 명령받아서 콜리전을 nocollision으로 전환


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PUZZLESTAY3_API UStage1BlockingVolumeComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	UStage1BlockingVolumeComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void HandleStage1BlockingDisabledChanged(bool bDisabled);

	void ApplyBlockingDisabled(bool bDisabled);

};
