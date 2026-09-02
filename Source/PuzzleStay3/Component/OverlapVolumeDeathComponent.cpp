#include "Component/OverlapVolumeDeathComponent.h"
#include "Engine/Engine.h"

UOverlapVolumeDeathComponent::UOverlapVolumeDeathComponent()
{
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	SetCollisionResponseToAllChannels(ECR_Ignore);
	SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	SetGenerateOverlapEvents(true);
}

void UOverlapVolumeDeathComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(
		this,
		&UOverlapVolumeDeathComponent::HandleBeginOverlap);
}

void UOverlapVolumeDeathComponent::HandleBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	APawn* PlayerPawn = Cast<APawn>(OtherActor);
	if (!IsValid(PlayerPawn))
	{
		return;
	}

	if (ProcessedPawns.Contains(PlayerPawn))
	{
		return;
	}

	ProcessedPawns.Add(PlayerPawn);

	OnPlayerEnteredDeathVolume.Broadcast(PlayerPawn);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, 
			FString::Printf(TEXT("플레이어 오버랩 감지: %s"), *OtherActor->GetName()));
	}
}

void UOverlapVolumeDeathComponent::ResetDeathGuard(
	APawn* PlayerPawn)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	ProcessedPawns.Remove(PlayerPawn);
}