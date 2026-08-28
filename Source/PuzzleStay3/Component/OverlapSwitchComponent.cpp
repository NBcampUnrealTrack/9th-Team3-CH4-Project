#include "OverlapSwitchComponent.h"
#include "Net/UnrealNetwork.h"

UOverlapSwitchComponent::UOverlapSwitchComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true); //컴포넌트 리플리케이션 활성화
	bIsOverlapped = false;
}

void UOverlapSwitchComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UOverlapSwitchComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UOverlapSwitchComponent, bIsOverlapped);
}

void UOverlapSwitchComponent::HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	//서버 권한(Authority)에서만 상태 변경 수행
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	if (OtherActor && OtherActor != GetOwner())
	{
		//액터가 몇 개가 있든 0에서 1로 증가할때만 true를 반환
		OverlappedActorCount++;
		if (OverlappedActorCount > 0 && !bIsOverlapped)
		{
			bIsOverlapped = true;
			OnRep_IsOverlapped(); //서버에서는 자동 호출 안되기에 서버에서도 수동 호출 
		}
	}
}

void UOverlapSwitchComponent::HandleEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	if (OtherActor && OtherActor != GetOwner())
	{
		//오직 1에서 0으로 갈때만 false를 반환하므로 오버랩이 2개이상부터는 실행되지 않음
		OverlappedActorCount = FMath::Max(0, OverlappedActorCount - 1);
		if (OverlappedActorCount == 0 && bIsOverlapped)
		{
			bIsOverlapped = false;
			OnRep_IsOverlapped(); //서버에서는 자동 호출 안되기에 서버에서도 수동 호출 
		}
	}
}

void UOverlapSwitchComponent::OnRep_IsOverlapped()
{
	//상태 변경 시 델리게이트 브로드캐스트 (블루프린트/소유 액터 반응)
	OnOverlapStateChanged.Broadcast(bIsOverlapped);
}
