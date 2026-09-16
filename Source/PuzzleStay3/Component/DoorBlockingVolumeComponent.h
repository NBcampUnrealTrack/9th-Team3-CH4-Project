// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "DoorBlockingVolumeComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PUZZLESTAY3_API UDoorBlockingVolumeComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDoorBlockingVolumeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void SetDoorFullyOpened(bool bOpened);

	UFUNCTION(BlueprintCallable)
	void SetBlockingEnabled(bool bEnabled);
ㅈㅈㅇㅈ
};
