// Fill out your copyright notice in the Description page of Project Settings.


#include "Stage3BlockingVolumeComponent.h"

#include "Core/GameState/PS3GameStateS3.h"


UStage3BlockingVolumeComponent::UStage3BlockingVolumeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetCollisionProfileName(TEXT("BlockAll"));
}

void UStage3BlockingVolumeComponent::BeginPlay()
{
	Super::BeginPlay();

	APS3GameStateS3* GS = GetWorld()->GetGameState<APS3GameStateS3>();
	if (!IsValid(GS)) return;

	GS->OnStage3BlockingVolumeDisabled.AddDynamic(
		this,
		&UStage3BlockingVolumeComponent::HandleStage3BlockingDisabledChanged
	);

	ApplyBlockingDisabled(GS->IsStage3BlockingVolumeDisabled());
}

void UStage3BlockingVolumeComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	UWorld* World = GetWorld();
	if (!World) return;
	
	APS3GameStateS3* GS = World->GetGameState<APS3GameStateS3>();
	if (!IsValid(GS)) return;
	
	GS->OnStage3BlockingVolumeDisabled.RemoveDynamic(
			this,
			&UStage3BlockingVolumeComponent::HandleStage3BlockingDisabledChanged
		);

	
}

void UStage3BlockingVolumeComponent::HandleStage3BlockingDisabledChanged(bool bDisabled)
{
	ApplyBlockingDisabled(bDisabled);
}

void UStage3BlockingVolumeComponent::ApplyBlockingDisabled(bool bDisabled)
{
	SetCollisionEnabled(bDisabled ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
	SetHiddenInGame(bDisabled);
}
