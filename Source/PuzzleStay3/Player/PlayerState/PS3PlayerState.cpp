#include "PS3PlayerState.h"

#include "Net/UnrealNetwork.h"

APS3PlayerState::APS3PlayerState()
{
	bReplicates = true;
}

void APS3PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentLifeCount);
	DOREPLIFETIME(ThisClass, bIsDead);
	DOREPLIFETIME(ThisClass, PlayerRole);
}

void APS3PlayerState::SetCurrentLifeCount(const int32 NewLifeCount)
{
	if (!HasAuthority())
	{
		return;
	}

	const int32 ClampedLifeCount = FMath::Max(0, NewLifeCount);
	if (CurrentLifeCount == ClampedLifeCount)
	{
		return;
	}

	CurrentLifeCount = ClampedLifeCount;
	OnLifeCountChanged.Broadcast(CurrentLifeCount);
}

void APS3PlayerState::SetIsDead(const bool bNewIsDead)
{
	if (!HasAuthority() || bIsDead == bNewIsDead)
	{
		return;
	}

	bIsDead = bNewIsDead;
	OnDeadStateChanged.Broadcast(bIsDead);
}

void APS3PlayerState::SetPlayerRole(const EPS3PlayerRole NewRole)
{
	if (!HasAuthority() || PlayerRole == NewRole)
	{
		return;
	}

	PlayerRole = NewRole;
	OnPlayerRoleChanged.Broadcast(PlayerRole);
}

void APS3PlayerState::OnRep_LifeChanged()
{
	OnLifeCountChanged.Broadcast(CurrentLifeCount);
}

void APS3PlayerState::OnRep_DeadChanged()
{
	OnDeadStateChanged.Broadcast(bIsDead);
}

void APS3PlayerState::OnRep_PlayerRole()
{
	OnPlayerRoleChanged.Broadcast(PlayerRole);
}
