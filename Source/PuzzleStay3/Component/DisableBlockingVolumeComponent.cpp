// Fill out your copyright notice in the Description page of Project Settings.


#include "DisableBlockingVolumeComponent.h"


UDisableBlockingVolumeComponent::UDisableBlockingVolumeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool UDisableBlockingVolumeComponent::IsOverlapped() const
{
	return bIsOverlapped;
}

void UDisableBlockingVolumeComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &UDisableBlockingVolumeComponent::OnOverlapDeathVolume);
}

void UDisableBlockingVolumeComponent::OnOverlapDeathVolume(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
	)
{
	bIsOverlapped = true;
}
