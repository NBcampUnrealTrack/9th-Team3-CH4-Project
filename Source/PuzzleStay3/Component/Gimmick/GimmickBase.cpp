#include "GimmickBase.h"

#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"

AGimmickBase::AGimmickBase()
{
 	bReplicates = true;
	SetReplicateMovement(false);
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

}

void AGimmickBase::BeginPlay()
{
	Super::BeginPlay();	
}

bool AGimmickBase::bCanInteract(const APawn* InteractingPawn) const
{
	return bIsActive && !bIsCompleted && IsValid(InteractingPawn);
}

void AGimmickBase::HandleInteraction(APawn* InteractingPawn)
{
	if (!HasAuthority() || !bCanInteract(InteractingPawn))
	{
		return;
	}

	BP_OnInteraction(InteractingPawn);
}

void AGimmickBase::ResetGimmick()
{
	if (!HasAuthority())
	{
		return;
	}

	bIsActive = true;
	bIsCompleted = false;

	OnRep_IsActive();
	OnRep_IsCompleted();
}

void AGimmickBase::OnRep_IsActive()
{
	BP_OnActiveStateChanged(bIsActive);
}

void AGimmickBase::OnRep_IsCompleted()
{
	BP_OnCompleted();
}

void AGimmickBase::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGimmickBase, bIsActive);
	DOREPLIFETIME(AGimmickBase, bIsCompleted);
}

