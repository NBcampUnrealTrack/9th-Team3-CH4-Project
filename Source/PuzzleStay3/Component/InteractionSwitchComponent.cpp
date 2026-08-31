#include "InteractionSwitchComponent.h"
#include "Net/UnrealNetwork.h"

UInteractionSwitchComponent::UInteractionSwitchComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
}

void UInteractionSwitchComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractionSwitchComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UInteractionSwitchComponent, bIsActivated);
}

void UInteractionSwitchComponent::Interact()
{
	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		Server_Interact();
	}
	else
	{
		Server_Interact_Implementation();
	}
}

void UInteractionSwitchComponent::Server_Interact_Implementation()
{
	// 서버 권한으로 상태 토글 (On <-> Off)
	bIsActivated = !bIsActivated;
    
	// 서버 로컬 브로드캐스트
	OnRep_IsActivated();
}

bool UInteractionSwitchComponent::Server_Interact_Validate()
{
	return true; // 거리 검증 등이 필요한 경우 조건 추가
}

void UInteractionSwitchComponent::OnRep_IsActivated()
{
	// 상태 변동 시 델리게이트 브로드캐스트 (램프 Light Color 연출 등)
	OnInteractionStateChanged.Broadcast(bIsActivated);
	
	// C++ 디버그 메시지 출력
	if (GEngine)
	{
		FString Message = bIsActivated ? TEXT("Interaction Lamp: Activated (BLUE)") : TEXT("Interaction Lamp: Deactivated (RED)");
		FColor Color = bIsActivated ? FColor::Blue : FColor::Red;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, Color, Message);
	}
}
