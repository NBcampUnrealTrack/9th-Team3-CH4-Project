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
    SetCollisionEnabled(
        bHasCollision
            ? ECollisionEnabled::QueryAndPhysics
            : ECollisionEnabled::NoCollision);
}

void URandomCollisionTrapComponent::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(URandomCollisionTrapComponent, bHasCollision);
}
