#include "Component/RandomCollisionTrapComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

URandomCollisionTrapComponent::URandomCollisionTrapComponent()
{
	SetIsReplicatedByDefault(true);

	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void URandomCollisionTrapComponent::BeginPlay()
{
    Super::BeginPlay();
    
    SpawnPlatforms();
    ApplyCollisionLayoutToPlatforms();
    
    //테스트용--
    if (GetOwner() && GetOwner()->HasAuthority())
    {
        const TArray<bool> TestCollisionLayout =
        {
            true, false,
            false, true,
            true, false,
            true, false,
            false, true,
            false, true,
            true, false,
            false, true,
            true, false,
            false, true
        };

        ApplyCollisionLayout(TestCollisionLayout);
    }
    //테스트용--
}

void URandomCollisionTrapComponent::ApplyCollisionLayout(
    const TArray<bool>& InCollisionLayout)
{
    if (!GetOwner() || !GetOwner()->HasAuthority())
    {
        return;
    }

    const int32 ExpectedPlatformCount = RowCount * ColumnCount;
    if (InCollisionLayout.Num() != ExpectedPlatformCount)
    {
        return;
    }

    SpawnPlatforms();

    CollisionLayout.SetNum(ExpectedPlatformCount);
    for (int32 Index = 0; Index < ExpectedPlatformCount; ++Index)
    {
        CollisionLayout[Index] = InCollisionLayout[Index] ? 1 : 0;
    }

    ApplyCollisionLayoutToPlatforms();
    GetOwner()->ForceNetUpdate();
}

void URandomCollisionTrapComponent::OnRep_CollisionLayout()
{
    SpawnPlatforms();
    ApplyCollisionLayoutToPlatforms();
}

void URandomCollisionTrapComponent::SpawnPlatforms()
{
    if (PlatformMeshes.Num() > 0 || PlatformCollisions.Num() > 0)
    {
        return;
    }

    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return;
    }

    const int32 PlatformCount = RowCount * ColumnCount;
    PlatformMeshes.Reserve(PlatformCount);
    PlatformCollisions.Reserve(PlatformCount);

    for (int32 Row = 0; Row < RowCount; ++Row)
    {
        for (int32 Column = 0; Column < ColumnCount; ++Column)
        {
            const int32 Index = Row * ColumnCount + Column;
            const FVector LocalLocation =
                StartLocalLocation
                + RowLocalOffset * Row
                + ColumnLocalOffset * Column;

            const FName MeshName(*FString::Printf(TEXT("PlatformMesh_%02d"), Index));
            UStaticMeshComponent* PlatformMesh =
                NewObject<UStaticMeshComponent>(Owner, MeshName);

            PlatformMesh->SetStaticMesh(PlatformMeshAsset);
            PlatformMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            PlatformMesh->SetupAttachment(this);
            PlatformMesh->SetRelativeLocation(LocalLocation);
            PlatformMesh->RegisterComponent();

            const FName CollisionName(
                *FString::Printf(TEXT("PlatformCollision_%02d"), Index));
            UBoxComponent* PlatformCollision =
                NewObject<UBoxComponent>(Owner, CollisionName);

            PlatformCollision->SetupAttachment(PlatformMesh);

            if (PlatformMeshAsset)
            {
                const FBoxSphereBounds MeshBounds = PlatformMeshAsset->GetBounds();

                PlatformCollision->SetBoxExtent(MeshBounds.BoxExtent);
                PlatformCollision->SetRelativeLocation(MeshBounds.Origin);
            }

            PlatformCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            PlatformCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
            PlatformCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
            PlatformCollision->RegisterComponent();

            PlatformMeshes.Add(PlatformMesh);
            PlatformCollisions.Add(PlatformCollision);
        }
    }
}

void URandomCollisionTrapComponent::ApplyCollisionLayoutToPlatforms()
{
    if (CollisionLayout.Num() != PlatformCollisions.Num())
    {
        return;
    }

    for (int32 Index = 0; Index < PlatformCollisions.Num(); ++Index)
    {
        PlatformCollisions[Index]->SetCollisionEnabled(
            CollisionLayout[Index] == 1
                ? ECollisionEnabled::QueryAndPhysics
                : ECollisionEnabled::NoCollision);
    }
}

void URandomCollisionTrapComponent::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(URandomCollisionTrapComponent, CollisionLayout);
}
