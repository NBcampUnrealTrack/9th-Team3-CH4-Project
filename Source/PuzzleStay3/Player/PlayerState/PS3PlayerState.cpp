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
	DOREPLIFETIME(ThisClass, VoiceChatState);
	DOREPLIFETIME(ThisClass, bIsVoiceObjectHeld);
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

void APS3PlayerState::InitializeLifeCount(const int32 NewLifeCount)
{
	if (!HasAuthority())
	{
		return;
	}

	bIsRespawning = false;
	SetIsDead(false);
	SetCurrentLifeCount(NewLifeCount);
}

bool APS3PlayerState::TryConsumeLife()
{
	if (!HasAuthority() || bIsDead || bIsRespawning || CurrentLifeCount <= 0)
	{
		return false;
	}

	SetCurrentLifeCount(CurrentLifeCount - 1);

	if (CurrentLifeCount <= 0)
	{
		SetIsDead(true);
		return true;
	}

	bIsRespawning = true;
	return true;
}

void APS3PlayerState::FinishRespawn()
{
	if (!HasAuthority())
	{
		return;
	}

	bIsRespawning = false;
}

void APS3PlayerState::SetVoiceChatState(
	const EVoiceChatState NewState
)
{
	if (!HasAuthority() || VoiceChatState == NewState)
	{
		return;
	}

	VoiceChatState = NewState;

	// 서버에서는 OnRep가 호출되지 않으므로 직접 방송합니다.
	OnVoiceChatStateChanged.Broadcast(VoiceChatState);
}

void APS3PlayerState::SetVoiceObjectHeld(
	const bool bNewIsHeld
)
{
	if (!HasAuthority() || bIsVoiceObjectHeld == bNewIsHeld)
	{
		return;
	}

	bIsVoiceObjectHeld = bNewIsHeld;
	OnVoiceObjectHeldChanged.Broadcast(bIsVoiceObjectHeld);
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

void APS3PlayerState::OnRep_VoiceChatState()
{
	OnVoiceChatStateChanged.Broadcast(VoiceChatState);
}

void APS3PlayerState::OnRep_VoiceObjectHeld()
{
	OnVoiceObjectHeldChanged.Broadcast(bIsVoiceObjectHeld);
}
