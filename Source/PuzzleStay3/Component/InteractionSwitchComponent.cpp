#include "InteractionSwitchComponent.h"
#include "Data/Enum/InteractionState.h"
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
		
		if (bIsActivated && InteractingActor != Requestor)
		{
			FString OwnerName = InteractingActor ? InteractingActor->GetName() : TEXT("다른 플레이어");
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
	if (!CanInteract_Implementation(Requestor))	return false;
	
	OnInteractionSuccessed.Broadcast(bIsInteractedGimmick);
	
	if (bIsOtherInteractionGimmick)
	{
		OnCosmeticInteractionSuccessed.Broadcast();
		return true;
	}
	
	if (!GetOwner() || !GetOwner()->HasAuthority())	return false;
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
		GetWorld()->GetTimerManager().ClearTimer(AutoDisableTimerHandle);

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
			&UInteractionSwitchComponent::ResetSwitch, // 시간 만료 시 꺼짐 함수 호출
			AutoDisableTime,
			false
		);

		UE_LOG(LogTemp, Warning, TEXT("[InteractionSwitch] 타이머 시작: [%s] 스위치가 %.1f초 후 자동으로 꺼집니다."),
		       *GetOwner()->GetName(), AutoDisableTime);
	}
}

void UInteractionSwitchComponent::ResetSwitch()
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		GetWorld()->GetTimerManager().ClearTimer(AutoDisableTimerHandle);
		
		if (!bMultiInteractionState)
		{
			// 점유 중이던 플레이어의 InteractionState 해제
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

void UInteractionSwitchComponent::OnRep_IsActivated()
{
	OnSwitchActivatedChanged.Broadcast(bIsActivated);
}
