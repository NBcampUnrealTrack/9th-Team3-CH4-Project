#include "Object/Dumbbell.h"

#include "Components/BoxComponent.h"
#include "Data/Delegates/UIDelegatesSubsystem.h"
#include "Data/Enum/PS3InteractionNotifyType.h"
#include "Net/UnrealNetwork.h"
#include "Player/Character/PS3PlayerCharacter.h"

ADumbbell::ADumbbell()
{
	PrimaryActorTick.bCanEverTick = false;

	DumbbellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DumbbellMesh"));
	SetRootComponent(DumbbellMesh);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f)); // 기본 감지 범위
	
	SetReplicatingMovement(true);
	bReplicates = true;

	DumbbellMesh->SetSimulatePhysics(false);
	DumbbellMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
}

void ADumbbell::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ADumbbell::OnTriggerBeginOverlap);
		TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ADumbbell::OnTriggerEndOverlap);
	}
}

void ADumbbell::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APS3PlayerCharacter* PlayerCharacter = Cast<APS3PlayerCharacter>(OtherActor))
	{
		if (PlayerCharacter->IsLocallyControlled())
		{
			ShowInteractionUI(true);
		}
	}
}

void ADumbbell::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (APS3PlayerCharacter* PlayerCharacter = Cast<APS3PlayerCharacter>(OtherActor))
	{
		if (PlayerCharacter->IsLocallyControlled())
		{
			if (IsHeld() && HoldingPlayer == PlayerCharacter)
			{
				return;
			}

			ShowInteractionUI(false);
		}
	}
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

	if (TriggerBox)
	{
		TriggerBox->SetCollisionEnabled(bIsHeld ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryOnly);
	}

	if (HoldingPlayer && HoldingPlayer->IsLocallyControlled())
	{
		ShowInteractionUI(true);
	}
	else if (!bIsHeld && PreviousHoldingPlayer && PreviousHoldingPlayer->IsLocallyControlled())
	{
		ShowInteractionUI(false);
	}

	PreviousHoldingPlayer = HoldingPlayer;
}

void ADumbbell::ShowInteractionUI(bool bShow)
{
	if (bShow)
	{
		if (!IsHeld())
		{
			PS3_BROADCAST_TO_MVVM_OneParams(OnInteractionNotifyAddRequested_UI, EPS3InteractionNotifyType::Interact);
			PS3_BROADCAST_TO_MVVM_OneParams(OnInteractionNotifyRemoveRequested_UI, EPS3InteractionNotifyType::Drop);
		}
		else
		{
			PS3_BROADCAST_TO_MVVM_OneParams(OnInteractionNotifyRemoveRequested_UI, EPS3InteractionNotifyType::Interact);
			PS3_BROADCAST_TO_MVVM_OneParams(OnInteractionNotifyAddRequested_UI, EPS3InteractionNotifyType::Drop);
		}
	}
	else
	{
		PS3_BROADCAST_TO_MVVM_OneParams(OnInteractionNotifyRemoveRequested_UI, EPS3InteractionNotifyType::Interact);
		PS3_BROADCAST_TO_MVVM_OneParams(OnInteractionNotifyRemoveRequested_UI, EPS3InteractionNotifyType::Drop);
	}
}