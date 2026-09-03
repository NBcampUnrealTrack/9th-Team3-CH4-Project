#include "Component/FakeDeathTrapComponent.h"

#include "Components/StaticMeshComponent.h"

UFakeDeathTrapComponent::UFakeDeathTrapComponent()
{
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UFakeDeathTrapComponent::BeginPlay()
{
	Super::BeginPlay();

	ResolveVisualMesh();
	
	if (GetNetMode() != NM_DedicatedServer)
	{
		SetLocalVisibility(false);
	}
}

void UFakeDeathTrapComponent::ApplyLocalFakeTrapState(bool bShouldBeVisible)
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	ResolveVisualMesh();

	bIsVisibleForLocalPlayer = bShouldBeVisible;

	SetLocalVisibility(bShouldBeVisible);
}

void UFakeDeathTrapComponent::ResolveVisualMesh()
{
	if (!IsValid(VisualMesh) && GetOwner())
	{
		VisualMesh = GetOwner()->FindComponentByClass<UStaticMeshComponent>();
	}
}

void UFakeDeathTrapComponent::SetLocalVisibility(bool bShouldBeVisible)
{
	if (IsValid(VisualMesh))
	{
		VisualMesh->SetVisibility(bShouldBeVisible, true);
	}
}
