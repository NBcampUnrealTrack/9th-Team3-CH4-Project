#include "Object/Dumbbell.h"

#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

ADumbbell::ADumbbell()
{
	PrimaryActorTick.bCanEverTick = false;

	DumbbellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DumbbellMesh"));
	SetRootComponent(DumbbellMesh);

	// 멀티플레이 및 물리 연동 설정
	SetReplicatingMovement(true);
	bReplicates = true;

	// 콜리전 기본값
	DumbbellMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
}

void ADumbbell::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADumbbell, bIsHeld);
}

bool ADumbbell::TryInteract(AActor* Requestor)
{
	// 서버 권한 검증 및 누가 들고 있는지 확인 
	if (!HasAuthority() || !Requestor || bIsHeld) return false;

	ACharacter* RequestorCharacter = Cast<ACharacter>(Requestor);
	if (!RequestorCharacter || !RequestorCharacter->GetMesh()) return false;

	HoldingCharacter = RequestorCharacter;
	bIsHeld = true;

	// 손 소켓에 부착
	AttachToComponent(
		RequestorCharacter->GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		GrabSocketName
	);
	
	// 캐릭터와 너무 붙지 않도록 GrabOffset 적용 (X, Y, Z 거리 조절)
	SetActorRelativeLocation(GrabOffset);
	SetActorRelativeRotation(GrabRotationOffset);
	
	// 서버 로컬에서는 OnRep이 자동 호출되지 않으므로 수동 호출
	OnRep_bIsHeld();

	return true;
}

void ADumbbell::TryDrop()
{
	if (!HasAuthority() || !bIsHeld) return;

	bIsHeld = false;
	HoldingCharacter = nullptr;
	
	// 부착 해제
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// 라인트레이스로 아래쪽 바닥 위치 탐색
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0.0f, 0.0f, 500.0f); // 아래쪽 5m 탐색

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // 자기 자신 제외
	if (HoldingCharacter)
	{
		QueryParams.AddIgnoredActor(HoldingCharacter); // 들고 있던 캐릭터 제외
	}

	// ECC_WorldStatic 및 WorldDynamic 채널 탐색
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams))
	{
		// 메쉬 바운드 절반 높이만큼 띄워서 바닥에 파묻히지 않게 보정
		const float HalfHeight = DumbbellMesh->Bounds.BoxExtent.Z;
		SetActorLocation(HitResult.ImpactPoint + FVector(0.0f, 0.0f, HalfHeight));
	}
	
	

	OnRep_bIsHeld();
}

void ADumbbell::OnRep_bIsHeld()
{
	DumbbellMesh->SetCollisionEnabled(
		bIsHeld ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryOnly
	);
}
