#include "RandomCollisionTrap.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

ARandomCollisionTrap::ARandomCollisionTrap()
{
	bReplicates = true;
	SetReplicateMovement(false);

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	SetRootComponent(PlatformMesh);
	
	PlatformMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ARandomCollisionTrap::SetPlatformCollision(bool bEnableCollision)
{
	if (!HasAuthority())
	{
		return;
	}

	bHasPlatformCollision = bEnableCollision;
	ApplyCollisionState();
	ForceNetUpdate();
}

void ARandomCollisionTrap::OnRep_HasPlatformCollision()
{
	ApplyCollisionState();
}

void ARandomCollisionTrap::ApplyCollisionState()
{
	PlatformMesh->SetCollisionEnabled(
		bHasPlatformCollision
			? ECollisionEnabled::QueryAndPhysics
			: ECollisionEnabled::NoCollision);
	
	if (bHasPlatformCollision)
	{
		PlatformMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		PlatformMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	}
}

void ARandomCollisionTrap::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARandomCollisionTrap, bHasPlatformCollision);
}