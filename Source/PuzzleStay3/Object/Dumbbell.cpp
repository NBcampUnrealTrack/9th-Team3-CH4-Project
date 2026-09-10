#include "Object/Dumbbell.h"
#include "Net/UnrealNetwork.h"
#include "Player/Character/PS3PlayerCharacter.h"

ADumbbell::ADumbbell()
{
	PrimaryActorTick.bCanEverTick = false;

	DumbbellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DumbbellMesh"));
	SetRootComponent(DumbbellMesh);

	// 멀티플레이 설정
	SetReplicatingMovement(true);
	bReplicates = true;

	// 네트워크 위치 오차 및 캐릭터 튕김을 방지하기 위해 물리 시뮬레이션 비활성화
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

	// 요청자가 올바른 캐릭터가 아니면 불가
	if (!IsValid(PlayerCharacter))
	{
		return false;
	}

	// 이미 누군가 덤벨을 들고 있다면 상호작용 불가
	if (IsValid(HoldingPlayer))
	{
		return false;
	}

	// 요청한 플레이어가 이미 다른 덤벨을 들고 있다면 상호작용 불가
	if (IsValid(PlayerCharacter->GetHeldDumbbell()))
	{
		return false;
	}

	return true;
}

bool ADumbbell::Interact_Implementation(AActor* Requestor)
{
	// 서버 권한 재검증
	if (!HasAuthority())
	{
		return false;
	}

	APS3PlayerCharacter* PlayerCharacter = Cast<APS3PlayerCharacter>(Requestor);

	if (!IsValid(PlayerCharacter))
	{
		return false;
	}

	// 서버 최종 판정 조건 재확인 (동시 요청 방지)
	if (!CanInteract_Implementation(Requestor))
	{
		return false;
	}

	// 검증을 통과하면 기존 TryInteract 실행하여 부착
	return TryInteract(PlayerCharacter);
}

bool ADumbbell::TryInteract(APS3PlayerCharacter* Requestor)
{
	// 서버 권한 검증 및 누가 들고 있는지 확인 
	if (!HasAuthority() || !Requestor) return false;

	// 이미 누군가 들고 있으면 상호작용 불가
	if (HoldingPlayer != nullptr) return false;

	// Requestor의 CarryAnchor 소켓 가져오기
	USceneComponent* CarryAnchor = Requestor->GetCarryAnchor();
	if (!IsValid(CarryAnchor)) return false;

	// 점유 플레이어 지정 및 소켓 부착
	HoldingPlayer = Requestor;

	AttachToComponent(
		CarryAnchor,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale
	);

	// 캐릭터와 너무 붙지 않도록 GrabOffset 적용 (X, Y, Z 거리 조절)
	SetActorRelativeLocation(GrabOffset);
	SetActorRelativeRotation(GrabRotationOffset);

	// 서버 로컬에서는 OnRep이 자동 호출되지 않으므로 수동 호출
	OnRep_HoldingPlayer();

	return true;
}

bool ADumbbell::TryDrop(APS3PlayerCharacter* Requestor)
{
	if (!HasAuthority() || !IsValid(Requestor)) return false;
	if (HoldingPlayer != Requestor) return false;

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// 플레이어 전방에서 아래로 라인트레이스해 바닥으로 내려놓기
	FVector Start = Requestor->GetActorLocation() + (Requestor->GetActorForwardVector() * 80.0f);
	FVector End = Start - FVector(0.0f, 0.0f, 500.0f); // 아래쪽 5m 탐색

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(Requestor);
	QueryParams.bTraceComplex = true; // 메쉬의 복합 콜리전 대신 정밀 메쉬 바닥에 히트되도록 설정

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByObjectType(HitResult, Start, End, ObjectQueryParams, QueryParams))
	{
		SetActorLocationAndRotation(HitResult.ImpactPoint, FRotator::ZeroRotator);
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
