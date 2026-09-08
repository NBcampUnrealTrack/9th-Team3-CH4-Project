#include "Component/FakeDeathTrapComponent.h"

#include "Components/StaticMeshComponent.h"

UFakeDeathTrapComponent::UFakeDeathTrapComponent()
{
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	SetCollisionResponseToAllChannels(ECR_Ignore);
	SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	SetGenerateOverlapEvents(true);
}

void UFakeDeathTrapComponent::BeginPlay()
{
	Super::BeginPlay();

	ResolveVisualMesh();
	
	OnComponentBeginOverlap.AddDynamic(
	this,
	&ThisClass::HandleBeginOverlap);
	
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

void UFakeDeathTrapComponent::HandleBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	APawn* PlayerPawn = Cast<APawn>(OtherActor);

	if (!bIsVisibleForLocalPlayer ||
		!IsValid(PlayerPawn) ||
		!PlayerPawn->IsLocallyControlled())
	{
		return;
	}

	OnFakeDeathTrapOverlapped.Broadcast(PlayerPawn);
}
