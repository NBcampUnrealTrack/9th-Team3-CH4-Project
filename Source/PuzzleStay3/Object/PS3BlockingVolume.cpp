// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3BlockingVolume.h"

#include "Components/BrushComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PuzzleStay3/Core/GameMode/PS3GameModeBase.h"

void APS3BlockingVolume::BeginPlay()
{
	Super::BeginPlay();

	APS3GameModeBase* GameMode = Cast<APS3GameModeBase>(UGameplayStatics::GetGameMode(this));
	if (!IsValid(GameMode)) return;

	BlockingVolumeDisabledHandle = 
		GameMode->OnBlockingVolumeDisabled.AddUObject(this,&APS3BlockingVolume::BlockingVolumeDisabled);
}

void APS3BlockingVolume::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	APS3GameModeBase* GameMode = Cast<APS3GameModeBase>(UGameplayStatics::GetGameMode(this));
	if (IsValid(GameMode))
	{
		GameMode->OnBlockingVolumeDisabled.Remove(BlockingVolumeDisabledHandle);
	}
	BlockingVolumeDisabledHandle.Reset();

	Super::EndPlay(EndPlayReason);
}

void APS3BlockingVolume::BlockingVolumeDisabled()
{
	SetActorEnableCollision(false);

	if (UBrushComponent* BrushComp = GetBrushComponent())
	{
		BrushComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}