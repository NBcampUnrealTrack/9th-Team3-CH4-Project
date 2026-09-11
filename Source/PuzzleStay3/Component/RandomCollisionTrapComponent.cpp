#include "Component/RandomCollisionTrapComponent.h"
#include "Core/GameMode/PS3GameModeS2.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

URandomCollisionTrapComponent::URandomCollisionTrapComponent()
{
	SetIsReplicatedByDefault(true);

	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetCollisionResponseToAllChannels(ECR_Ignore);
	SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

void URandomCollisionTrapComponent::OnRegister()
{
    Super::OnRegister();

    UWorld* World = GetWorld();
    AActor* Owner = GetOwner();
    if (!World || !World->IsGameWorld() || !Owner || !Owner->HasAuthority())
    {
        return;
    }

    APS3GameModeS2* GameMode =
        Cast<APS3GameModeS2>(World->GetAuthGameMode());
    if (!GameMode)
    {
        return;
    }

    GameMode->RegisterRandomCollisionTrapCompo(this);
    bRegisteredToStage2GameMode = true;
}

void URandomCollisionTrapComponent::BeginPlay()
{
    Super::BeginPlay();

    if (!GetOwner() || !GetOwner()->HasAuthority())
    {
        return;
    }
    
    OnComponentBeginOverlap.AddDynamic(
    this,
    &ThisClass::HandleBeginOverlap);

    ApplyCollisionFromStage2GameMode();
}

void URandomCollisionTrapComponent::EndPlay(
    const EEndPlayReason::Type EndPlayReason)
{
    if (bRegisteredToStage2GameMode)
    {
        if (UWorld* World = GetWorld())
        {
            if (APS3GameModeS2* GameMode =
                Cast<APS3GameModeS2>(World->GetAuthGameMode()))
            {
                GameMode->UnregisterRandomCollisionTrapCompo(this);
            }
        }

        bRegisteredToStage2GameMode = false;
    }

    Super::EndPlay(EndPlayReason);
}

void URandomCollisionTrapComponent::ApplyCollisionFromStage2GameMode()
{
    UWorld* World = GetWorld();
    if (!World || !World->IsGameWorld() || LayoutIndex == INDEX_NONE)
    {
        return;
    }

    APS3GameModeS2* GameMode =
        Cast<APS3GameModeS2>(World->GetAuthGameMode());
    if (!GameMode)
    {
        return;
    }

    const TArray<bool> CollisionLayout =
        GameMode->GetRandomCollisionLayoutResults();
    if (!CollisionLayout.IsValidIndex(LayoutIndex))
    {
        return;
    }

    ApplyCollisionState(CollisionLayout[LayoutIndex]);
}

void URandomCollisionTrapComponent::ApplyCollisionState(
    const bool bShouldHaveCollision)
{
    if (!GetOwner() || !GetOwner()->HasAuthority())
    {
        return;
    }

    bHasCollision = bShouldHaveCollision;
    UpdateCollisionState();

    GetOwner()->ForceNetUpdate();
}

void URandomCollisionTrapComponent::OnRep_HasCollision()
{
    UpdateCollisionState();
}

void URandomCollisionTrapComponent::UpdateCollisionState()
{
    if (bHasCollision)
    {
        SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
        SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }
    else
    {
        SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
        SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }

    SetGenerateOverlapEvents(true);
}

void URandomCollisionTrapComponent::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(URandomCollisionTrapComponent, bHasCollision);
}

void URandomCollisionTrapComponent::HandleBeginOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!GetOwner() || !GetOwner()->HasAuthority()) { return; }
    
    if (bHasCollision) { return; }

    APawn* PlayerPawn = Cast<APawn>(OtherActor);
    
    if (!IsValid(PlayerPawn)) { return; }

    MulticastFakePlatformOverlapped(PlayerPawn);
}

void URandomCollisionTrapComponent::MulticastFakePlatformOverlapped_Implementation(
        APawn* PlayerPawn)
{
    if (GetNetMode() == NM_DedicatedServer || !IsValid(PlayerPawn)) { return; }

    OnFakePlatformOverlapped.Broadcast(PlayerPawn);
}