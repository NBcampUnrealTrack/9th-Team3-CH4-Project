#include "GimmickBase.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

AGimmickBase::AGimmickBase()
{
 	bReplicates = true;
	SetReplicateMovement(false);
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	GimmickMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GimmickMesh"));
	GimmickMesh->SetupAttachment(Root);
	// 각 기믹의 실제 충돌은 전용 컴포넌트가 담당합니다.
	GimmickMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
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

