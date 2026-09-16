// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorBlockingVolumeComponent.h"

UDoorBlockingVolumeComponent::UDoorBlockingVolumeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UDoorBlockingVolumeComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetCollisionObjectType(ECC_WorldStatic);
	SetCollisionResponseToAllChannels(ECR_Ignore);
	SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	SetHiddenInGame(true);
	SetBlockingEnabled(true);
}

void UDoorBlockingVolumeComponent::SetDoorFullyOpened(bool bOpened)
{
	SetBlockingEnabled(!bOpened);
}

void UDoorBlockingVolumeComponent::SetBlockingEnabled(bool bEnabled)
{
	if (bEnabled)
	{
		SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

