#include "Object/Dumbbell.h"

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

void ADumbbell::Grab(USceneComponent* TargetParent)
{
	if (!HasAuthority() || !TargetParent) return;
	
	// 잡았을 때는 물리를 끄고 캐릭터의 손/소켓 위치에 부착
	DumbbellMesh->SetSimulatePhysics(false);
	TargetParent->UpdateComponentToWorld();
	AttachToComponent(TargetParent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void ADumbbell::Drop()
{
	if (!HasAuthority()) return;

	// 놓았을 때는 부착을 해제하고 물리를 다시 켜서 바닥으로 떨어지게 함
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	DumbbellMesh->SetSimulatePhysics(true);
}

