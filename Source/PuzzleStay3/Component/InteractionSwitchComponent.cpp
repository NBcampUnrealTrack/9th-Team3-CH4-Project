
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

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (bRegisterToGameMode)
		{
			if (UWorld* World = GetWorld())
			{
				if (APS3GameModeBase* GM = Cast<APS3GameModeBase>(World->GetAuthGameMode()))
				{
					GM->RegisterInteractionSwitch(this);
				}
			}
		}
		// BeginPlay 시점 자동 작동 체크
		if (bStartTimerOnBeginPlay)
		{
			bIsActivated = true;
			OnRep_IsActivated(); // 켜짐 상태 알림 (델리게이트 쏘기)
			StartDisableTimer(); // 시간이 지나면 꺼지는 타이머 가동
		}
	}
}

void UInteractionSwitchComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 액터/컴포넌트 파괴 시 GameMode에서 해제
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		GetWorld()->GetTimerManager().ClearTimer(AutoDisableTimerHandle);

		if (bRegisterToGameMode)
		{
			if (UWorld* World = GetWorld())
			{
				if (APS3GameModeBase* GM = Cast<APS3GameModeBase>(World->GetAuthGameMode()))
				{
					GM->UnregisterInteractionSwitch(this);
				}
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

bool UInteractionSwitchComponent::CanInteract_Implementation(AActor* Requestor) const
{
	// 스위치가 비활성화 상태일 때만 상호작용 가능
	return !bIsActivated;
}

bool UInteractionSwitchComponent::Interact_Implementation(AActor* Requestor)
{
	return TryInteract(Requestor);
}

bool UInteractionSwitchComponent::TryInteract(AActor* Requestor)
{
	if(bIsEscapeDoor == false) return false;
	
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}
	bIsActivated = !bIsActivated;

	// 플레이어가 상호작용했을 때 스위치가 켜졌고, 타이머 사용 옵션이 켜져 있다면
	if (bIsActivated && bUseAutoDisableTimer)
	{
		StartDisableTimer();
	}

	OnRep_IsActivated();
	OnInteractionSuccessed.Broadcast();

	return true;
}

void UInteractionSwitchComponent::StartDisableTimer()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			AutoDisableTimerHandle,
			this,
			&UInteractionSwitchComponent::ResetSwitch, // 시간 만료 시 꺼짐 함수 호출
			AutoDisableTime,
			false
		);
	}
}

void UInteractionSwitchComponent::ResetSwitch()
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		GetWorld()->GetTimerManager().ClearTimer(AutoDisableTimerHandle);
		bIsActivated = false;
		OnRep_IsActivated();
	}
}

void UInteractionSwitchComponent::OnRep_IsActivated()
{
	// 스위치 상태 변경(True/False) 시 델리게이트 쏘기
	OnSwitchActivatedChanged.Broadcast(bIsActivated);
	if (GEngine)
	{
		FString Message = TEXT("TEST");
		FColor Color = FColor::Red;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, Color, Message);
	}
}
