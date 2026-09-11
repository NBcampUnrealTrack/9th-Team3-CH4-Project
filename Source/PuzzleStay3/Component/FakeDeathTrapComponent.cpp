#include "Component/FakeDeathTrapComponent.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Core/GameMode/PS3GameModeS3.h"
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
	
	APS3GameModeS3* GameMode = Cast<APS3GameModeS3>(GetWorld()->GetAuthGameMode());
	if (!GameMode) return;

	GameMode->RegisterFakeDeathTrapComponent(this);
	bRegisteredToGameModeS3 = true;
}

void UFakeDeathTrapComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (bRegisteredToGameModeS3 == false) return;
	
	APS3GameModeS3* GameMode = Cast<APS3GameModeS3>(GetWorld()->GetAuthGameMode());
	if (!GameMode) return;

	GameMode->UnregisterFakeDeathTrapComponent(this);
	bRegisteredToGameModeS3 = false;
}

void UFakeDeathTrapComponent::ApplyLocalFakeTrapState(bool bShouldBeVisible)
{
	if (GetNetMode() == NM_DedicatedServer)	{ return; }

	ResolveVisualMesh();

	bIsVisibleForLocalPlayer = bShouldBeVisible;

	SetLocalVisibility(bShouldBeVisible);
	
	SetCollisionEnabled(
		bShouldBeVisible
			? ECollisionEnabled::QueryOnly
			: ECollisionEnabled::NoCollision);
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
