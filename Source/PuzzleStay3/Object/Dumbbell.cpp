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

	// 물리 및 콜리전 기본값
	DumbbellMesh->SetSimulatePhysics(true);
	DumbbellMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
}

void ADumbbell::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADumbbell, bIsHeld);
}

bool ADumbbell::TryInteract(AActor* Requestor)
{
	// 서버 권한 검증 (InteractionSwitchComponent와 동일 패턴)
	if (!HasAuthority() || !Requestor)
	{
		return false;
	}
	
	// 이미 누가 들고 있으면 새로 잡지 못함
	if (bIsHeld)
	{
		return false;
	}
	
	ACharacter* RequestorCharacter = Cast<ACharacter>(Requestor);
	if (!RequestorCharacter || !RequestorCharacter->GetMesh())
	{
		return false;
	}
	
	HoldingCharacter = RequestorCharacter;
	bIsHeld = true;

	// 물리를 끄고 손 소켓에 부착
	DumbbellMesh->SetSimulatePhysics(false);
	DumbbellMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttachToComponent(
		RequestorCharacter->GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		GrabSocketName
	);
	
	// 서버 로컬에서는 OnRep이 자동 호출되지 않으므로 수동 호출
	OnRep_bIsHeld();

	return true;
}

void ADumbbell::TryDrop()
{
	if (!HasAuthority() || !bIsHeld)
	{
		return;
	}

	bIsHeld = false;
	HoldingCharacter = nullptr;

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	DumbbellMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DumbbellMesh->SetSimulatePhysics(true);

	OnRep_bIsHeld();
}

void ADumbbell::OnRep_bIsHeld()
{
	// 클라이언트에서는 부착/물리 처리를 직접 안 해도 되지만
	// (Attach 자체는 리플리케이트되는 Attachment 시스템으로 따라옴),
	// 들린 동안 겹치는 오버랩 판정 등 시각/판정 보정이 필요하면 여기서 처리
	// 예: 저울판 Overlap 무시, 아웃라인 하이라이트 제거 등
}


