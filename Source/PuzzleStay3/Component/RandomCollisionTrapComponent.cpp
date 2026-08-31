#include "Component/RandomCollisionTrapComponent.h"
#include "Object/RandomCollisionTrap.h"

//테스트용 생성
void URandomCollisionTrapComponent::BeginPlay()
{
    Super::BeginPlay();

    if (!GetOwner() || !GetOwner()->HasAuthority())
    {
        return;
    }

    InitializePlatforms(InitialSeed);
}

URandomCollisionTrapComponent::URandomCollisionTrapComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URandomCollisionTrapComponent::InitializePlatforms(int32 InSeed)
{
    if (!GetOwner() || !GetOwner()->HasAuthority())
    {
        return;
    }

    SpawnPlatforms();
    ApplyRandomCollisionLayout(InSeed);
}

void URandomCollisionTrapComponent::ResetPlatforms(int32 InSeed)
{
    if (!GetOwner() || !GetOwner()->HasAuthority())
    {
        return;
    }

    ApplyRandomCollisionLayout(InSeed);
}

void URandomCollisionTrapComponent::SpawnPlatforms()
{
    if (Platforms.Num() > 0 || !TrapClass)
    {
        return;
    }

    AActor* Owner = GetOwner();
    UWorld* World = GetWorld();

    for (int32 Row = 0; Row < RowCount; ++Row)
    {
        for (int32 Column = 0; Column < ColumnCount; ++Column)
        {
            const FVector LocalLocation =
                StartLocalLocation
                + RowLocalOffset * Row
                + ColumnLocalOffset * Column;

            const FVector WorldLocation =
                Owner->GetActorTransform().TransformPosition(LocalLocation);

            ARandomCollisionTrap* NewPlatfrom =
                World->SpawnActor<ARandomCollisionTrap>(
                    TrapClass,
                    WorldLocation,
                    Owner->GetActorRotation());

            if (NewPlatfrom)
            {
                NewPlatfrom->AttachToActor(
                    Owner,
                    FAttachmentTransformRules::KeepWorldTransform);

                Platforms.Add(NewPlatfrom);
            }
        }
    }
}

void URandomCollisionTrapComponent::ApplyRandomCollisionLayout(int32 InSeed)
{
    if (Platforms.Num() != RowCount * ColumnCount)
    {
        return;
    }

    FRandomStream RandomStream(InSeed);

    for (int32 Row = 0; Row < RowCount; ++Row)
    {
        // 0이면 왼쪽 발판 안전, 1이면 오른쪽 발판 안전
        const int32 SafeColumn = RandomStream.RandRange(0, 1);

        for (int32 Column = 0; Column < ColumnCount; ++Column)
        {
            const int32 PanelIndex = Row * ColumnCount + Column;
            const bool bEnableCollision = (Column == SafeColumn);

            Platforms[PanelIndex]->SetPlatformCollision(bEnableCollision);
        }
    }
}