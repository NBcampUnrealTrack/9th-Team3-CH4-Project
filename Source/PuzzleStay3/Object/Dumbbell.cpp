#include "Object/Dumbbell.h"

#include "GameFramework/Character.h"
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
	)

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

	// 부착 해제
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// 플레이어 전방에서 아래로 라인트레이스해 바닥으로 내려놓기
	FVector Start = Requestor->GetActorLocation() + (Requestor->GetActorForwardVector() * 50.0f);
	FVector End = Start - FVector(0.0f, 0.0f, 500.0f); // 아래쪽 5m 탐색

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // 자기 자신 제외
	QueryParams.AddIgnoredActor(Requestor);

	// ECC_WorldStatic 및 WorldDynamic 채널 탐색
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams))
	{
		// 메쉬 바운드 절반 높이만큼 띄워서 바닥에 파묻히지 않게 보정
		const float HalfHeight = DumbbellMesh->Bounds.BoxExtent.Z;
		SetActorLocation(HitResult.ImpactPoint + FVector(0.0f, 0.0f, HalfHeight));
	}

	HoldingPlayer = nullptr;
	OnRep_HoldingPlayer();
	
	return true;
}

void ADumbbell::OnRep_HoldingPlayer()
{
	
	bool bIsHeld = (HoldingPlayer != nullptr);
	DumbbellMesh->SetCollisionEnabled(
		bIsHeld ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryOnly
	);
}
