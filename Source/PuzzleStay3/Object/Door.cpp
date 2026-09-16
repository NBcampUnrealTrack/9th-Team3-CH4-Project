#include "Object/Door.h"

#include "Component/InteractionSwitchComponent.h"
#include "Component/OverlapSwitchComponent.h"
#include "Core/GameMode/PS3GameModeBase.h"
#include "Core/GameState/PS3GameStateBase.h"
#include "EngineUtils.h"
#include "Component/DoorBlockingVolumeComponent.h"
#include "Core/GameState/PS3GameStateS4.h"
#include "Net/UnrealNetwork.h"

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(RootComponent);
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	DoorBlockingVolumeComponent = FindComponentByClass<UDoorBlockingVolumeComponent>(); //민웅 수정
	
	InitialRelativeLocation = DoorMesh->GetRelativeLocation();

	if (HasAuthority())
	{
		UWorld* World = GetWorld();
		if (!World) return;
		switch (DoorType)
		{
			// 전 스테이지 공통 최종 탈출문 (GameStateBase 연동)
		case EDoorType::StageAllFinalDoor:
			if (APS3GameStateBase* GS = World->GetGameState<APS3GameStateBase>())
			{
				GS->OnEscapeDoorOpened.AddDynamic(this, &ADoor::OnOpenDoor);
				UE_LOG(LogTemp, Warning, TEXT("[Door] StageAllFinalDoor -> GameStateBase 바인딩 완료!"));

				if (GS->IsEscapeDoorOpened())
				{
					OnOpenDoor(true);
				}
			}
			break;
			// Stage 4 첫 번째 문 (저울 기믹 정답 완료 시 GameStateS4 연동)
		case EDoorType::Stage4FirstDoor:
			if (APS3GameStateS4* GS = World->GetGameState<APS3GameStateS4>())
			{
				GS->OnStage4FirstDoorOpenedChanged.AddDynamic(this, &ADoor::OnOpenDoor);
				UE_LOG(LogTemp, Warning, TEXT("[Door] Stage4FirstDoor -> GameStateS4 바인딩 완료!"));

				if (GS->IsStage4FirstDoorOpened())
				{
					OnOpenDoor(true);
				}
			}
			break;

			//  일반 스위치/발판 연동문 (GameState 없이 DoorID <-> SwitchID 직통 연동)
		case EDoorType::Stage1NormalDoor:
		case EDoorType::Stage5NormalDoor:
		default:
			BindSwitchesByID();
			break;
		}
	}
}

void ADoor::BindSwitchesByID()
{
	UWorld* World = GetWorld();
	if (!World) return;

	LinkedOverlapSwitches.Empty();
	LinkedInteractionSwitches.Empty();

	// 월드 내의 모든 액터를 순회하며 컴포넌트 ID 매칭
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor) continue;

		// 1. OverlapSwitchComponent 검사
		if (UOverlapSwitchComponent* OverlapComp = Actor->FindComponentByClass<UOverlapSwitchComponent>())
		{
			if (OverlapComp->SwitchID == DoorID)
			{
				OverlapComp->OnOverlapStateChanged.AddUObject(this, &ADoor::OnLinkedSwitchStateChanged);
				LinkedOverlapSwitches.Add(OverlapComp);
			}
		}

		// 2. InteractionSwitchComponent 검사
		if (UInteractionSwitchComponent* InteractComp = Actor->FindComponentByClass<UInteractionSwitchComponent>())
		{
			if (InteractComp->SwitchID == DoorID)
			{
				InteractComp->OnSwitchActivatedChanged.AddUObject(this, &ADoor::OnLinkedSwitchStateChanged);
				LinkedInteractionSwitches.Add(InteractComp);
			}
		}
	}

	// 바인딩 직후 초기 상태 평가
	EvaluateDoorState();
}

void ADoor::EvaluateDoorState()
{
	int32 TotalLinkedSwitches = LinkedOverlapSwitches.Num() + LinkedInteractionSwitches.Num();
	if (TotalLinkedSwitches == 0) return;

	int32 ActiveCount = 0;

	// 매칭된 오버랩 스위치 활성화 상태 카운트
	for (const auto& OverlapComp : LinkedOverlapSwitches)
	{
		if (IsValid(OverlapComp) && OverlapComp->IsOverlapped())
		{
			ActiveCount++;
		}
	}

	// 매칭된 인터렉션 스위치 활성화 상태 카운트
	for (const auto& InteractComp : LinkedInteractionSwitches)
	{
		if (IsValid(InteractComp) && InteractComp->IsActivated())
		{
			ActiveCount++;
		}
	}

	// 매칭된 모든 스위치가 활성화되어야 문이 열림 (1개인 경우 1개만 켜지면 됨)
	bool bShouldBeOpen = (ActiveCount == TotalLinkedSwitches);

	if (bIsOpen != bShouldBeOpen)
	{
		bIsOpen = bShouldBeOpen;
		
		// 민웅 수정
		if (!bIsOpen)
		{
			if (DoorBlockingVolumeComponent)
			{
				DoorBlockingVolumeComponent->SetDoorFullyOpened(false);
			}
		}
		// 민웅 수정 끝
		
		OnIsDoorOpen.Broadcast(bIsOpen); // << 추가
		BroadcastExitDoorOpenIfNeeded(bIsOpen); // 현준 수정
		
		UE_LOG(LogTemp, Warning, TEXT("[Door] DoorID %d 문 상태 변경: bIsOpen = %s (활성화: %d / 전체: %d)"),
			DoorID, bIsOpen ? TEXT("True") : TEXT("False"), ActiveCount, TotalLinkedSwitches);
	}
}

void ADoor::OnLinkedSwitchStateChanged(bool bIsActivated)
{
	if (!HasAuthority()) return;

	EvaluateDoorState();
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 1. 목표 상대 위치 계산 (InitialRelativeLocation 및 TargetRelativeLocation 기반)
	FVector TargetPos = bIsOpen ? (InitialRelativeLocation + TargetRelativeLocation) : InitialRelativeLocation;
	FVector CurrentPos = DoorMesh->GetRelativeLocation();

	FVector DeltaVec = TargetPos - CurrentPos;
	float RemainingDistance = DeltaVec.Size();

	// 2. 오차 범위 이내 도달 시 최종 위치 고정
	if (RemainingDistance <= KINDA_SMALL_NUMBER)
	{
		DoorMesh->SetRelativeLocation(TargetPos);
		
		//민웅 수정
		if (bIsOpen)
		{
			if (DoorBlockingVolumeComponent)
			{
				DoorBlockingVolumeComponent->SetDoorFullyOpened(true);
			}
		}
		//민웅 수정 끝
		return;
	}

	FVector Direction = DeltaVec / RemainingDistance;

	// 3. VInterpTo 기반 지수 감속 이동량 계산
	float VInterpStep = RemainingDistance * FMath::Min(DeltaTime * OpenSpeed, 1.0f);

	// 4. 후반부 최소 보장 속도 
	float MinStep = MinSpeed * DeltaTime;

	// 5. 지수 감속과 최소 속도 중 더 큰 속도 채택 후 남은 거리 제한
	float FinalStep = FMath::Min(RemainingDistance, FMath::Max(VInterpStep, MinStep));

	// 6. 상대 위치 업데이트
	DoorMesh->SetRelativeLocation(CurrentPos + Direction * FinalStep);
}

void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADoor, bIsOpen);
}

void ADoor::OnOpenDoor(bool bOpened)
{
	if (!HasAuthority()) return;
	bIsOpen = bOpened;
	
	//민웅 수정
	if (!bIsOpen)
	{
		if (DoorBlockingVolumeComponent)
		{
			DoorBlockingVolumeComponent->SetDoorFullyOpened(false);
		}
	}
	//민웅 수정 끝
	
	OnIsDoorOpen.Broadcast(bIsOpen); 
	BroadcastExitDoorOpenIfNeeded(bIsOpen); // 현준 수정

	//test dnjsqls
	UE_LOG(LogTemp, Warning, TEXT("[Door] 서버: OnOpenDoor 호출됨! bIsOpen = %s"), bIsOpen ? TEXT("True") : TEXT("False"));
}

void ADoor::OnRep_bIsOpen()
{
	//민웅 수정
	if (!bIsOpen)
	{
		if (DoorBlockingVolumeComponent)
		{
			DoorBlockingVolumeComponent->SetDoorFullyOpened(false);
		}
	}
	//민웅 수정 끝
	
	// 클라이언트 지점: Replication 수신 시 방송
	OnIsDoorOpen.Broadcast(bIsOpen); 
	BroadcastExitDoorOpenIfNeeded(bIsOpen); // 현준 수정

	//test dnjsqls
	UE_LOG(LogTemp, Log, TEXT("[Door] 클라이언트: OnRep_bIsOpen 호출됨 (클라이언트 동기화 완료)"));
}

void ADoor::BroadcastExitDoorOpenIfNeeded(bool bOpened) // 현준 수정
{
	if (!bIsExitDoor) // 현준 수정
	{
		return; // 현준 수정
	}

	OnIsExitDoorOpen.Broadcast(bOpened); // 현준 수정
}
