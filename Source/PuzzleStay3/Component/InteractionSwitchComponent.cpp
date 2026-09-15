#include "InteractionSwitchComponent.h"

#include "Data/Delegates/UIDelegatesSubsystem.h"
#include "Data/Enum/InteractionState.h"
#include "Data/Enum/PS3InteractionNotifyType.h"
#include "Net/UnrealNetwork.h"
#include "Player/Character/PS3PlayerCharacter.h"
#include "PuzzleStay3/Player/PlayerState/PS3PlayerState.h"
#include "PuzzleStay3/Core/GameMode/PS3GameModeBase.h"

UInteractionSwitchComponent::UInteractionSwitchComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UInteractionSwitchComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!bRegisterToGameMode || !GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		if (APS3GameModeBase* GM = World->GetAuthGameMode<APS3GameModeBase>())
		{
			GM->RegisterInteractionSwitch(this);
		}
	}
}

void UInteractionSwitchComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(AutoDisableTimerHandle);
			StopAutoDisableUITimer(false);

			if (bRegisterToGameMode)
			{
				if (APS3GameModeBase* GM = World->GetAuthGameMode<APS3GameModeBase>())
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
	DOREPLIFETIME(UInteractionSwitchComponent, bIsInteractedGimmick);
	DOREPLIFETIME(UInteractionSwitchComponent, bIsEscapeDoor);
	DOREPLIFETIME(UInteractionSwitchComponent, InteractingActor);
	DOREPLIFETIME(UInteractionSwitchComponent, bMultiInteractionState);
	DOREPLIFETIME(UInteractionSwitchComponent, bIsOtherInteractionGimmick);
}

bool UInteractionSwitchComponent::CanInteract_Implementation(AActor* Requestor) const
{
	if (bIsLocked)
	{
		UE_LOG(LogTemp, Warning, TEXT("[InteractionSwitch] 상호작용 실패: 완료되었거나 잠긴 스위치입니다."));
		return false;
	}

	if (!bMultiInteractionState)
	{
		if (const APS3PlayerCharacter* Character = Cast<APS3PlayerCharacter>(Requestor))
		{
			if (const APS3PlayerState* PS = Character->GetPlayerState<APS3PlayerState>())
			{
				if (!bIsActivated && PS->IsInteracting())
				{
					UE_LOG(LogTemp, Warning, TEXT("[InteractionSwitch] 상호작용 실패: %s 님이 이미 다른 상호작용을 진행 중입니다."),
						*Requestor->GetName());
					return false;
				}
			}
		}

		if (bIsActivated && IsValid(InteractingActor) && InteractingActor != Requestor)
		{
			FString OwnerName = InteractingActor->GetName();
			UE_LOG(LogTemp, Warning, TEXT("[InteractionSwitch] 상호작용 실패: 현재 %s 님이 이미 상호작용 중인 스위치입니다."), *OwnerName);
			return false;
		}
	}
	return true;
}

bool UInteractionSwitchComponent::Interact_Implementation(AActor* Requestor)
{
	return TryInteract(Requestor);
}

bool UInteractionSwitchComponent::TryInteract(AActor* Requestor)
{
	if (bIsEscapeDoor == false) return false;
	if (!CanInteract_Implementation(Requestor)) return false;
	if (!GetOwner() || !GetOwner()->HasAuthority()) return false;

	OnInteractionSuccessed.Broadcast(bIsInteractedGimmick);

	if (bIsOtherInteractionGimmick)
	{
		OnCosmeticInteractionSuccessed.Broadcast();
		return true;
	}

	bIsInteractedGimmick = true;

	APS3PlayerCharacter* Character = Cast<APS3PlayerCharacter>(Requestor);
	APS3PlayerState* PS = Character ? Character->GetPlayerState<APS3PlayerState>() : nullptr;

	bIsActivated = !bIsActivated;

	if (bIsActivated)
	{
		InteractingActor = Requestor;
		if (!bMultiInteractionState && PS)
		{
			PS->SetInteractionState(EInteractionState::IsInteracting);
		}

		if (bUseAutoDisableTimer)
		{
			StartDisableTimer();
		}

		UE_LOG(LogTemp, Warning, TEXT("[InteractionSwitch] ON: %s 님이 [%s] 스위치를 켰습니다."),
			*Requestor->GetName(), *GetOwner()->GetName());
	}
	else
	{
		if (!bMultiInteractionState && PS)
		{
			PS->SetInteractionState(EInteractionState::IsNotInteracting);
		}

		InteractingActor = nullptr;
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(AutoDisableTimerHandle);
		}
		StopAutoDisableUITimer(true);

		UE_LOG(LogTemp, Warning, TEXT("[InteractionSwitch] OFF: %s 님이 [%s] 스위치를 껐습니다."),
			*Requestor->GetName(), *GetOwner()->GetName());
	}

	OnRep_IsActivated();
	OnCosmeticInteractionSuccessed.Broadcast();

	return true;
}

void UInteractionSwitchComponent::StartDisableTimer()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			AutoDisableTimerHandle,
			this,
			&UInteractionSwitchComponent::ResetSwitch,
			AutoDisableTime,
			false
		);
		StartAutoDisableUITimer();

		UE_LOG(LogTemp, Warning, TEXT("[InteractionSwitch] 타이머 시작: [%s] 스위치가 %.1f초 후 자동으로 꺼집니다."),
			*GetOwner()->GetName(), AutoDisableTime);
	}
}

void UInteractionSwitchComponent::ResetSwitch()
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(AutoDisableTimerHandle);
		}
		StopAutoDisableUITimer(true);

		if (!bMultiInteractionState && IsValid(InteractingActor))
		{
			if (APS3PlayerCharacter* Character = Cast<APS3PlayerCharacter>(InteractingActor))
			{
				if (APS3PlayerState* PS = Character->GetPlayerState<APS3PlayerState>())
				{
					PS->SetInteractionState(EInteractionState::IsNotInteracting);
				}
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("[InteractionSwitch] 타이머 만료: [%s] 스위치가 시간 초과로 리셋되었습니다."),
			*GetOwner()->GetName());

		bIsActivated = false;
		InteractingActor = nullptr;
		OnRep_IsActivated();
	}
}

void UInteractionSwitchComponent::StartAutoDisableUITimer()
{
	if (AutoDisableTimerUIType == EPS3TimerUIType::None)
	{
		return;
	}

	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AutoDisableUITimerHandle);
		BroadcastAutoDisableTimerRemaining();
		World->GetTimerManager().SetTimer(
			AutoDisableUITimerHandle,
			this,
			&UInteractionSwitchComponent::BroadcastAutoDisableTimerRemaining,
			0.1f,
			true
		);
	}
}

void UInteractionSwitchComponent::StopAutoDisableUITimer(bool bBroadcastFinished)
{
	bool bWasUITimerActive = false;

	if (UWorld* World = GetWorld())
	{
		bWasUITimerActive = World->GetTimerManager().IsTimerActive(AutoDisableUITimerHandle);
		World->GetTimerManager().ClearTimer(AutoDisableUITimerHandle);
	}

	if (bBroadcastFinished && bWasUITimerActive && AutoDisableTimerUIType != EPS3TimerUIType::None)
	{
		NetMultiRPC_FinishAutoDisableTimerUI(AutoDisableTimerUIType);
	}
}

void UInteractionSwitchComponent::BroadcastAutoDisableTimerRemaining()
{
	if (AutoDisableTimerUIType == EPS3TimerUIType::None)
	{
		return;
	}

	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		const float RemainingTime = FMath::Max(
			0.0f,
			World->GetTimerManager().GetTimerRemaining(AutoDisableTimerHandle)
		);

		NetMultiRPC_BroadcastAutoDisableTimerUI(AutoDisableTimerUIType, RemainingTime);
	}
}

void UInteractionSwitchComponent::NetMultiRPC_BroadcastAutoDisableTimerUI_Implementation(
	EPS3TimerUIType TimerUIType,
	float RemainingTime)
{
	if (TimerUIType == EPS3TimerUIType::None)
	{
		return;
	}

	PS3_BROADCAST_TO_MVVM_TwoParams(OnGameTimer_UI, TimerUIType, RemainingTime);
}

void UInteractionSwitchComponent::NetMultiRPC_FinishAutoDisableTimerUI_Implementation(EPS3TimerUIType TimerUIType)
{
	if (TimerUIType == EPS3TimerUIType::None)
	{
		return;
	}

	PS3_BROADCAST_TO_MVVM_TwoParams(OnGameTimer_UI, TimerUIType, 0.0f);
}

void UInteractionSwitchComponent::SetLocked(bool bNewLocked)
{
	bIsLocked = bNewLocked;

	if (bIsLocked)
	{
		// 스위치가 잠기면 안내 UI 일괄 제거
		PS3_BROADCAST_TO_MVVM(OnInteractionNotifyResetRequested_UI);
	}
}

void UInteractionSwitchComponent::ShowInteractionUI(bool bShow)
{
	// 퍼즐이 완결되어 영구 잠긴 상태(bIsLocked)일 때만 UI 노출을 강제로 차단
	if (bIsLocked)
	{
		PS3_BROADCAST_TO_MVVM_OneParams(OnInteractionNotifyRemoveRequested_UI, EPS3InteractionNotifyType::Interact);
		return;
	}

	if (bShow)
	{
		// F키 안내 UI 추가 요청
		PS3_BROADCAST_TO_MVVM_OneParams(OnInteractionNotifyAddRequested_UI, EPS3InteractionNotifyType::Interact);
	}
	else
	{
		// F키 안내 UI 제거 요청
		PS3_BROADCAST_TO_MVVM_OneParams(OnInteractionNotifyRemoveRequested_UI, EPS3InteractionNotifyType::Interact);
	}
}

void UInteractionSwitchComponent::OnRep_IsActivated()
{
	OnSwitchActivatedChanged.Broadcast(bIsActivated);
}
