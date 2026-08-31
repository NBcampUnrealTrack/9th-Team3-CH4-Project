#include "InteractionSwitchComponent.h"
#include "Net/UnrealNetwork.h"
#include "PuzzleStay3/Core/GameMode/PS3GameModeBase.h"

UInteractionSwitchComponent::UInteractionSwitchComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UInteractionSwitchComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// 서버 권한을 가진 GameMode에 자신을 자동 등록
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (UWorld* World = GetWorld())
		{
			// GetGameMode() 대신 GetAuthGameMode()를 사용합니다.
			if (APS3GameModeBase* GM = Cast<APS3GameModeBase>(World->GetAuthGameMode()))
			{
				GM->RegisterInteractionSwitch(this);
			}
		}
	}
}

void UInteractionSwitchComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 액터/컴포넌트 파괴 시 GameMode에서 해제
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (UWorld* World = GetWorld())
		{
			// GetGameMode() 대신 GetAuthGameMode()를 사용합니다.
			if (APS3GameModeBase* GM = Cast<APS3GameModeBase>(World->GetAuthGameMode()))
			{
				GM->UnregisterInteractionSwitch(this);
			}
		}
	}

	Super::EndPlay(EndPlayReason);
}

void UInteractionSwitchComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInteractionSwitchComponent, bIsActivated);
}

bool UInteractionSwitchComponent::TryInteract(AActor* Requestor)
{
	// 서버 권한 검증
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}
	// 단순 토글 처리
	bIsActivated = !bIsActivated;

	// 서버에서 GameMode 및 알림 전달
	OnRep_IsActivated();

	return true;
}

void UInteractionSwitchComponent::OnRep_IsActivated()
{
	// GameMode 또는 문(Door) 액터로 상태 변경 이벤트 방송
	OnSwitchActivatedChanged.Broadcast(bIsActivated);
}
