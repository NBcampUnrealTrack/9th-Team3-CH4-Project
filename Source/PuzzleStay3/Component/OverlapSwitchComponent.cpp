#include "OverlapSwitchComponent.h"

#include "Core/GameMode/PS3GameModeBase.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Object/Dumbbell.h"

UOverlapSwitchComponent::UOverlapSwitchComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true); //컴포넌트 리플리케이션 활성화
}

void UOverlapSwitchComponent::BeginPlay()
{
	Super::BeginPlay();

	// 서버 권한일 때 GameMode에 자동 등록
	// if (GetOwner() && GetOwner()->HasAuthority())
	// {
	// 	if (UWorld* World = GetWorld())
	// 	{
	// 		if (APS3GameModeBase* GM = Cast<APS3GameModeBase>(World->GetAuthGameMode()))
	// 		{
	// 			GM->RegisterOverlapSwitch(this); // GameMode에 이 함수를 만들어두어야 합니다.
	// 		}
	// 	}
	// }

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

void UOverlapSwitchComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 서버 권한일 때 GameMode에서 자동 해제
	// if (GetOwner() && GetOwner()->HasAuthority())
	// {
	// 	if (UWorld* World = GetWorld())
	// 	{
	// 		if (APS3GameModeBase* GM = Cast<APS3GameModeBase>(World->GetAuthGameMode()))
	// 		{
	// 			GM->UnregisterOverlapSwitch(this);
	// 		}
	// 	}
	// }

	Super::EndPlay(EndPlayReason);
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

	// 캐릭터(플레이어) 감지
	// if (TargetActor->IsA<ACharacter>())
	// {
	// 	return true;
	// }

	// Dumbbell 감지
	if (TargetActor->IsA<ADumbbell>())
	{
		return true;
	}

	return false;
}

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
	//상태 변경 시 델리게이트 브로드캐스트 (GameMode 등이 들음)
	OnOverlapStateChanged.Broadcast(bIsOverlapped);

	if (GEngine)
	{
		FString Message = bIsOverlapped ? TEXT("Press On (C++)") : TEXT("Press Off (C++)");
		FColor Color = bIsOverlapped ? FColor::Green : FColor::Red;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, Color, Message);
	}
}
