// Fill out your copyright notice in the Description page of Project Settings.


#include "Stage1BlockingVolumeComponent.h"

#include "Core/GameState/PS3GameStateS1.h"
#include "Data/Delegates/UIDelegatesSubsystem.h"
#include "Data/Enum/PS3TextNotifyType.h"
#include "GameFramework/Pawn.h"

UStage1BlockingVolumeComponent::UStage1BlockingVolumeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetCollisionProfileName(TEXT("BlockAll"));
}

void UStage1BlockingVolumeComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentHit.AddDynamic(this, &ThisClass::HandleBlockingVolumeHit);

	APS3GameStateS1* GS = GetWorld()->GetGameState<APS3GameStateS1>();
	if (!IsValid(GS)) return;

	GS->OnStage1BlockingVolumeDisabled.AddDynamic(
		this,
		&UStage1BlockingVolumeComponent::HandleStage1BlockingDisabledChanged
	);

	ApplyBlockingDisabled(GS->IsStage1BlockingVolumeDisabled());
}

void UStage1BlockingVolumeComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UWorld* World = GetWorld();
	if (!World) return;

	APS3GameStateS1* GS = World->GetGameState<APS3GameStateS1>();

	if (!IsValid(GS)) return;
	GS->OnStage1BlockingVolumeDisabled.RemoveDynamic(
		this,
		&UStage1BlockingVolumeComponent::HandleStage1BlockingDisabledChanged
	);
	
	World->GetTimerManager().ClearTimer(BlockNotifyCooldownTimerHandle);
}

void UStage1BlockingVolumeComponent::HandleStage1BlockingDisabledChanged(bool bDisabled)
{
	ApplyBlockingDisabled(bDisabled);
}

void UStage1BlockingVolumeComponent::ApplyBlockingDisabled(bool bDisabled)
{
	SetCollisionEnabled(bDisabled ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
	SetHiddenInGame(bDisabled);
}

void UStage1BlockingVolumeComponent::HandleBlockingVolumeHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                                             UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                                                             const FHitResult& Hit)
{
	if (!bCanShowBlockNotify) return;

	APawn* PlayerPawn = Cast<APawn>(OtherActor);
	if (!IsValid(PlayerPawn)) return;
	if (!PlayerPawn->IsLocallyControlled()) return;

	PS3_BROADCAST_TO_MVVM_OneParams(OnTextNotify_UI, EPS3TextNotifyType::BlockVolume);

	bCanShowBlockNotify = false;

	GetWorld()->GetTimerManager().SetTimer
	(
		BlockNotifyCooldownTimerHandle, [this]() { bCanShowBlockNotify = true; },
		BlockNotifyCooldown,
		false
	);
}
