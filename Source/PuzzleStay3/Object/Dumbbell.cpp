#include "Object/Dumbbell.h"

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
	
}

void ADumbbell::TryDrop()
{
}

void ADumbbell::Server_TryInteract_Implementation()
{
}

void ADumbbell::Server_TryDropHeldObject_Implementation()
{
}

void ADumbbell::OnRep_bIsHeld()
{
	
}