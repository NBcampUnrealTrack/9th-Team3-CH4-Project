// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3BlockingVolume.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PuzzleStay3/Core/GameMode/PS3GameModeBase.h"


APS3BlockingVolume::APS3BlockingVolume()
{
	PrimaryActorTick.bCanEverTick = false;
	
	BoxCompo = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(BoxCompo);
	
	BoxCompo->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxCompo->SetCollisionProfileName(TEXT("BlockAll"));
}

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

	if (IsValid(BoxCompo))
	{
		BoxCompo->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}