
#include "DisableBlockingVolumeComponent.h"

#include "Core/GameMode/PS3GameModeBase.h"
#include "Kismet/GameplayStatics.h"


UDisableBlockingVolumeComponent::UDisableBlockingVolumeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDisableBlockingVolumeComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &UDisableBlockingVolumeComponent::OnOverlapDisableBlockingVolume);
}

void UDisableBlockingVolumeComponent::OnOverlapDisableBlockingVolume(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
	)
{
	if (bIsOverlapped) return;
	
	APawn* overlappedPawn = Cast<APawn>(OtherActor);
	if (!IsValid(overlappedPawn) || !overlappedPawn->IsPlayerControlled()) return;
	
	bIsOverlapped = true;
	
	APS3GameModeBase* GameMode = Cast<APS3GameModeBase>(UGameplayStatics::GetGameMode(this));
	if (!IsValid(GameMode)) return;
	
	//플레이어 오버랩 시 게임모드의 DisableBlockingVolume 함수를 호출
	GameMode->DisableBlockingVolume(EPS3StageNumber::Stage1);
	
}
