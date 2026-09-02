// Fill out your copyright notice in the Description page of Project Settings.


#include "Stage3BlockingVolumeComponent.h"

#include "Kismet/GameplayStatics.h"
#include "PuzzleStay3/Core/GameMode/PS3GameModeBase.h"


UStage3BlockingVolumeComponent::UStage3BlockingVolumeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetCollisionProfileName(TEXT("BlockAll"));
}

void UStage3BlockingVolumeComponent::BeginPlay()
{
	Super::BeginPlay();

	APS3GameModeBase* GameMode = Cast<APS3GameModeBase>(UGameplayStatics::GetGameMode(this));
	if (!IsValid(GameMode)) return;

	BlockingVolumeDisabledHandle =
		GameMode->OnBlockingVolumeDisabled.AddUObject(
			this, 
			&UStage3BlockingVolumeComponent::DisableStage3BlockingVolume
			);
}

void UStage3BlockingVolumeComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	APS3GameModeBase* GameMode = Cast<APS3GameModeBase>(UGameplayStatics::GetGameMode(this));
	if (IsValid(GameMode))
	{
		GameMode->OnBlockingVolumeDisabled.Remove(BlockingVolumeDisabledHandle);
	}

	BlockingVolumeDisabledHandle.Reset();

	Super::EndPlay(EndPlayReason);
}

void UStage3BlockingVolumeComponent::DisableStage3BlockingVolume(EPS3StageNumber StageNumber)
{
	if (StageNumber != EPS3StageNumber::Stage3) return;
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
