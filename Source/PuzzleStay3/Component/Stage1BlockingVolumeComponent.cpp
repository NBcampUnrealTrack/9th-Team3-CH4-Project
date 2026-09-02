// Fill out your copyright notice in the Description page of Project Settings.


#include "Stage1BlockingVolumeComponent.h"

#include "Kismet/GameplayStatics.h"


UStage1BlockingVolumeComponent::UStage1BlockingVolumeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetCollisionProfileName(TEXT("BlockAll"));
}

void UStage1BlockingVolumeComponent::BeginPlay()
{
	Super::BeginPlay();

	APS3GameModeBase* GameMode = Cast<APS3GameModeBase>(UGameplayStatics::GetGameMode(this));
	if (!IsValid(GameMode)) return;

	BlockingVolumeDisabledHandle =
		GameMode->OnBlockingVolumeDisabled.AddUObject(
			this, 
			&UStage1BlockingVolumeComponent::DisableStage1BlockingVolume
			);
}

void UStage1BlockingVolumeComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	APS3GameModeBase* GameMode = Cast<APS3GameModeBase>(UGameplayStatics::GetGameMode(this));
	if (IsValid(GameMode))
	{
		GameMode->OnBlockingVolumeDisabled.Remove(BlockingVolumeDisabledHandle);
	}

	BlockingVolumeDisabledHandle.Reset();

	Super::EndPlay(EndPlayReason);
}

void UStage1BlockingVolumeComponent::DisableStage1BlockingVolume(EPS3StageNumber StageNumber)
{
	if (StageNumber != EPS3StageNumber::Stage1) return;
	
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
