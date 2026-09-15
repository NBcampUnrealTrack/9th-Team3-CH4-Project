#include "Object/Dumbbell.h"

#include "Data/Delegates/UIDelegatesSubsystem.h"
#include "Data/Enum/PS3InteractionNotifyType.h"
#include "Net/UnrealNetwork.h"
#include "Player/Character/PS3PlayerCharacter.h"

ADumbbell::ADumbbell()
{
	PrimaryActorTick.bCanEverTick = false;

	DumbbellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DumbbellMesh"));
	SetRootComponent(DumbbellMesh);

	SetReplicatingMovement(true);
	bReplicates = true;

	DumbbellMesh->SetSimulatePhysics(false);
	DumbbellMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
}

void ADumbbell::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADumbbell, HoldingPlayer);
}

bool ADumbbell::CanInteract_Implementation(AActor* Requestor) const
{
	const APS3PlayerCharacter* PlayerCharacter = Cast<APS3PlayerCharacter>(Requestor);

	if (!IsValid(PlayerCharacter)) return false;
	if (IsValid(HoldingPlayer)) return false;
	if (IsValid(PlayerCharacter->GetHeldDumbbell())) return false;

	return true;
}

bool ADumbbell::Interact_Implementation(AActor* Requestor)
{
	if (!HasAuthority()) return false;

	APS3PlayerCharacter* PlayerCharacter = Cast<APS3PlayerCharacter>(Requestor);
	if (!IsValid(PlayerCharacter)) return false;

	if (!CanInteract_Implementation(Requestor)) return false;

	return TryInteract(PlayerCharacter);
}

bool ADumbbell::TryInteract(APS3PlayerCharacter* Requestor)
{
	if (!HasAuthority() || !Requestor) return false;
	if (HoldingPlayer != nullptr) return false;

	USceneComponent* CarryAnchor = Requestor->GetCarryAnchor();
	if (!IsValid(CarryAnchor)) return false;

	HoldingPlayer = Requestor;

	AttachToComponent(
		CarryAnchor,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale
	);

	SetActorRelativeLocation(GrabOffset);
	SetActorRelativeRotation(GrabRotationOffset);

	OnRep_HoldingPlayer();

	return true;
}

bool ADumbbell::TryDrop(APS3PlayerCharacter* Requestor)
{
	if (!HasAuthority() || !IsValid(Requestor)) return false;
	if (HoldingPlayer != Requestor) return false;

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	FVector Start = Requestor->GetActorLocation() + (Requestor->GetActorForwardVector() * 80.0f);
	FVector End = Start - FVector(0.0f, 0.0f, 500.0f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(Requestor);
	QueryParams.bTraceComplex = true;

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByObjectType(HitResult, Start, End, ObjectQueryParams, QueryParams))
	{
		FVector DropLocation = HitResult.ImpactPoint;

		if (DumbbellMesh)
		{
			float HalfHeight = DumbbellMesh->Bounds.BoxExtent.Z;
			DropLocation.Z += (HalfHeight + GroundZOffset);
		}

		SetActorLocationAndRotation(DropLocation, FRotator::ZeroRotator);
	}

	HoldingPlayer = nullptr;
	OnRep_HoldingPlayer();

	return true;
}

void ADumbbell::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	switch (DumbbellType)
	{
	case EDumbbellType::Heavy: Weight = 200.0f;
		break;
	case EDumbbellType::Medium: Weight = 180.0f;
		break;
	case EDumbbellType::Light: Weight = 150.0f;
		break;
	}
}

void ADumbbell::OnRep_HoldingPlayer()
{
	const bool bIsHeld = (HoldingPlayer != nullptr);
	DumbbellMesh->SetCollisionEnabled(bIsHeld ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndProbe);
}

void ADumbbell::ShowInteractionUI(bool bShow)
{
	if (bShow)
	{
		if (!IsHeld())
		{
			// 바닥에 위치 시: F키(Interact) 노출, G키(Drop) 제거
			PS3_BROADCAST_TO_MVVM_OneParams(OnInteractionNotifyAddRequested_UI, EPS3InteractionNotifyType::Interact);
			PS3_BROADCAST_TO_MVVM_OneParams(OnInteractionNotifyRemoveRequested_UI, EPS3InteractionNotifyType::Drop);
		}
		else
		{
			// 손에 들고 있을 시: F키(Interact) 제거, G키(Drop) 노출
			PS3_BROADCAST_TO_MVVM_OneParams(OnInteractionNotifyRemoveRequested_UI, EPS3InteractionNotifyType::Interact);
			PS3_BROADCAST_TO_MVVM_OneParams(OnInteractionNotifyAddRequested_UI, EPS3InteractionNotifyType::Drop);
		}
	}
	else
	{
		// 포커스/영역 이탈 시 두 UI 모두 제거 요청
		PS3_BROADCAST_TO_MVVM_OneParams(OnInteractionNotifyRemoveRequested_UI, EPS3InteractionNotifyType::Interact);
		PS3_BROADCAST_TO_MVVM_OneParams(OnInteractionNotifyRemoveRequested_UI, EPS3InteractionNotifyType::Drop);
	}
}