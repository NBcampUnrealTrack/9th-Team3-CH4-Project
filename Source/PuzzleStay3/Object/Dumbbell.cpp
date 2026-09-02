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
	DOREPLIFETIME(ADumbbell, HoldingCharacter);
}

bool ADumbbell::TryInteract(AActor* Requestor)
{
	// 서버 권한 검증 (InteractionSwitchComponent와 동일 패턴)
	if (!HasAuthority() || !Requestor) return false;

	// 이미 누가 들고 있으면 새로 잡지 못함
	if (bIsHeld) return false;

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

	// 서버 로컬에서는 OnRep이 자동 호출되지 않으므로 수동 호출
	OnRep_bIsHeld();

	return true;
}

void ADumbbell::TryDrop()
{
	if (!HasAuthority() || !bIsHeld) return;

	bIsHeld = false;
	HoldingCharacter = nullptr;

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	OnRep_bIsHeld();
}

void ADumbbell::OnRep_bIsHeld()
{
	DumbbellMesh->SetCollisionEnabled(
		bIsHeld ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryOnly
	);
}
