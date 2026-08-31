#include "OverlapSwitchComponent.h"
#include "GameFramework/Character.h"
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

	// C++로만 오버랩 이벤트를 자동으로 바인딩하는 핵심 로직
	AActor* Owner = GetOwner();
	if (Owner)
	{
		// Owner 액터에 붙어있는 RootComponent 또는 첫 번째 PrimitiveComponent(Collision, Box 등)를 찾음
		UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(Owner->GetRootComponent());
		if (!PrimitiveComp)
		{
			PrimitiveComp = Owner->FindComponentByClass<UPrimitiveComponent>();
		}
		if (PrimitiveComp)
		{
			// test dnjsqls
			PrimitiveComp->SetHiddenInGame(false);
			
			// C++ 이벤트 바인딩 (AddDynamic)
			PrimitiveComp->OnComponentBeginOverlap.AddDynamic(this, &UOverlapSwitchComponent::OnOwnerBeginOverlap);
			PrimitiveComp->OnComponentEndOverlap.AddDynamic(this, &UOverlapSwitchComponent::OnOwnerEndOverlap);
		}
	}
}

void UOverlapSwitchComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UOverlapSwitchComponent, bIsOverlapped);
}

//오버랩 검증
bool UOverlapSwitchComponent::IsValidOverlapActor(AActor* TargetActor) const
{
	if (!TargetActor || TargetActor == GetOwner())
	{
		return false;
	}

	// 1. 캐릭터(플레이어) 감지
	if (TargetActor->IsA<ACharacter>())
	{
		return true;
	}

	// 2. 만약 Dumbbell(무게 추) 등 특정 오브젝트도 발판을 누를 수 있어야 한다면
	// 해당 클래스 헤더를 포함 후 Cast/IsA 체크를 아래와 같이 추가할 수 있습니다.
	// if (TargetActor->IsA<ADumbbell>()) return true;

	return false;
}

// void UOverlapSwitchComponent::HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
// {
// 	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
//
// 	if (IsValidOverlapActor(OtherActor))
// 	{
// 		OverlappedActorCount++;
// 		if (OverlappedActorCount > 0 && !bIsOverlapped)
// 		{
// 			bIsOverlapped = true;
// 			OnRep_IsOverlapped();
// 		}
// 	}
// }

// void UOverlapSwitchComponent::HandleEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
// {
// 	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
//
// 	if (IsValidOverlapActor(OtherActor))
// 	{
// 		OverlappedActorCount = FMath::Max(0, OverlappedActorCount - 1);
// 		if (OverlappedActorCount == 0 && bIsOverlapped)
// 		{
// 			bIsOverlapped = false;
// 			OnRep_IsOverlapped(); 
// 		}
// 	}
// }

void UOverlapSwitchComponent::OnOwnerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                  const FHitResult& SweepResult)
{
	//서버 권한(Authority)에서만 상태 변경 수행
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	if (IsValidOverlapActor(OtherActor))
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

void UOverlapSwitchComponent::OnOwnerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	if (IsValidOverlapActor(OtherActor))
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

	if (GEngine)
	{
		FString Message = bIsOverlapped ? TEXT("Press On (C++)") : TEXT("Press Off (C++)");
		FColor Color = bIsOverlapped ? FColor::Green : FColor::Red;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, Color, Message);
	}
}
