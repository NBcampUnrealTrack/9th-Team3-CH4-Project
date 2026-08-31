#include "InteractionSwitchComponent.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"

UInteractionSwitchComponent::UInteractionSwitchComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
	bIsActivated = false;
	OccupyingPlayer = nullptr;
}

void UInteractionSwitchComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractionSwitchComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInteractionSwitchComponent, bIsActivated);
	DOREPLIFETIME(UInteractionSwitchComponent, OccupyingPlayer); // 점유 플레이어 동기화
}

// 플레이어가 지정된 역할(Player A인지, Player B인지) 조건을 만족하는지 판별
bool UInteractionSwitchComponent::IsRequestorAllowed(AActor* Requestor) const
{
	if (!Requestor) return false;

	// Requestor(Pawn/Character)의 PlayerController 수신
	APawn* Pawn = Cast<APawn>(Requestor);
	if (!Pawn) return false;

	APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
	if (!PC) return false;

	// Any인 경우 누구나 허용
	if (AllowedRole == EPlayerRole::Any) return true;
	
	// 데디케이티드 서버에서 접속 순서/ID 기반 구분 (예: 첫 번째 플레이어 = Player 1)
	// GameState의 PlayerArray 인덱스 또는 PlayerState의 PlayerId/Index로 판단
	int32 PlayerIndex = -1;
	if (UWorld* World = GetWorld())
	{
		if (AGameStateBase* GS = World->GetGameState())
		{
			for (int32 i = 0; i < GS->PlayerArray.Num(); ++i)
			{
				if (GS->PlayerArray[i] == PC->PlayerState)
				{
					PlayerIndex = i; // 0번 = 첫 번째 접속자(Player 1), 1번 = 두 번째 접속자(Player 2)
					break;
				}
			}
		}
	}

	if (AllowedRole == EPlayerRole::PlayerA) return PlayerIndex == 0;
	if (AllowedRole == EPlayerRole::PlayerB) return PlayerIndex == 1;

	return true;
}

#pragma region Toggle

void UInteractionSwitchComponent::Interact(AActor* Requestor)
{
	if (!Requestor) return;
	Server_Interact(Requestor);
}

bool UInteractionSwitchComponent::Server_Interact_Validate(AActor* Requestor)
{
	return Requestor != nullptr;
}

void UInteractionSwitchComponent::Server_Interact_Implementation(AActor* Requestor)
{
	APawn* RequestorPawn = Cast<APawn>(Requestor);
	if (!RequestorPawn) return;

	// 이 RPC를 실제로 호출한 Connection의 Controller가 맞는지 확인
	// (이 컴포넌트/Owner 액터의 NetConnection 기준)
	if (AController* SendingController = Cast<AController>(GetOwner()->GetOwner()))
	{
		if (SendingController->GetPawn() != RequestorPawn)
		{
			return; // 위조 시도
		}
	}
	
	// [1. 역할 검증] 지정된 플레이어 역할(PlayerA/B)이 맞는지 검사
	if (!IsRequestorAllowed(Requestor))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Rejected: Role Mismatch!"));
		}
		return;
	}

	// [2. 점유 검증] 이미 누군가 켜놓은(ON) 상태이고, 내가 점유자가 아니라면 다른 플레이어 차단
	if (bIsActivated && OccupyingPlayer != nullptr && OccupyingPlayer != Requestor)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow,
			                                 TEXT("Rejected: Already Occupied by another player!"));
		}
		return;
	}

	// [3. 토글 처리]
	bIsActivated = !bIsActivated;

	if (bIsActivated)
	{
		// 켜질 때: 요청자를 점유자로 등록
		OccupyingPlayer = Requestor;
	}
	else
	{
		// 꺼질 때: 점유 해제
		OccupyingPlayer = nullptr;
	}

	OnRep_IsActivated();
}


#pragma endregion

// #pragma region Hold
// void UInteractionSwitchComponent::StartInteract(AActor* Requestor)
// {
// 	if (!Requestor) return;
//
// 	// 로컬 호출 시 서버 RPC 전송
// 	Server_StartInteract(Requestor);
// }
//
// bool UInteractionSwitchComponent::Server_StartInteract_Validate(AActor* Requestor)
// {
// 	return Requestor != nullptr;
// }
//
// void UInteractionSwitchComponent::Server_StartInteract_Implementation(AActor* Requestor)
// {
// 	APawn* RequestorPawn = Cast<APawn>(Requestor);
// 	if (!RequestorPawn) return;
//
// 	if (AController* SendingController = Cast<AController>(GetOwner()->GetOwner()))
// 	{
// 		if (SendingController->GetPawn() != RequestorPawn)
// 		{
// 			return; 
// 		}
// 	}
//
// 	// [1단계 검증] 지정된 플레이어 역할(PlayerA 전용 등)이 아닌 경우 차단
// 	if (!IsRequestorAllowed(Requestor))
// 	{
// 		if (GEngine)
// 		{
// 			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red,
// 			                                 TEXT("Rejected: You are NOT Player 1 (Role Mismatch)!"));
// 		}
// 		return;
// 	}
//
// 	// [핵심 점유 검증] 
// 	// 이미 다른 플레이어가 점유 중이라면 다른 플레이어의 요청을 거부(무시)합니다[cite: 2].
// 	if (OccupyingPlayer != nullptr && OccupyingPlayer != Requestor)
// 	{
// 		if (GEngine)
// 		{
// 			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow,
// 			                                 TEXT("Interaction Rejected: Occupied by another player!"));
// 		}
// 		return;
// 	}
//
// 	// 비어있는 상태라면 요청한 플레이어를 점유자로 할당[cite: 2]
// 	OccupyingPlayer = Requestor;
// 	bIsActivated = true;
// 	OnRep_IsActivated();
// }
//
// void UInteractionSwitchComponent::StopInteract(AActor* Requestor)
// {
// 	if (!Requestor) return;
//
// 	Server_StopInteract(Requestor);
// }
//
// bool UInteractionSwitchComponent::Server_StopInteract_Validate(AActor* Requestor)
// {
// 	return Requestor != nullptr;
// }
//
// void UInteractionSwitchComponent::Server_StopInteract_Implementation(AActor* Requestor)
// {
// 	// 점유하고 있던 본인만 해제할 수 있음
// 	if (OccupyingPlayer == Requestor)
// 	{
// 		OccupyingPlayer = nullptr; // 점유 해제
// 		bIsActivated = false;
// 		OnRep_IsActivated();
// 	}
// }
// #pragma endregion

void UInteractionSwitchComponent::OnRep_IsActivated()
{
	// 상태 변동 시 델리게이트 브로드캐스트 (램프 Light Color 연출 등)
	OnInteractionStateChanged.Broadcast(bIsActivated);

	// C++ 디버그 메시지 출력
	if (GEngine)
	{
		FString Message = bIsActivated
			                  ? FString::Printf(
				                  TEXT("Activated by [%s]"),
				                  OccupyingPlayer ? *OccupyingPlayer->GetName() : TEXT("None"))
			                  : TEXT("Deactivated (Free)");
		
		FColor Color = bIsActivated ? FColor::Blue : FColor::Red;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, Color, Message);
	}
}
