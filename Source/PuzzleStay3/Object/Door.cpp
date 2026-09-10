#include "Object/Door.h"

#include "Component/InteractionSwitchComponent.h"
#include "Component/OverlapSwitchComponent.h"
#include "Core/GameMode/PS3GameModeBase.h"
#include "Core/GameState/PS3GameStateBase.h"
#include "EngineUtils.h"
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

	InitialRelativeLocation = DoorMesh->GetRelativeLocation();

	if (HasAuthority())
	{
		// 일반 스테이지 문: GameState 없이 ID 매칭 직통 바인딩 수행
		if (DoorType != EDoorType::StageAllFinalDoor)
		{
			BindSwitchesByID();
		}
		
		// 최종 탈출문: GameState 델리게이트 활용
		else
		{
			if (UWorld* World = GetWorld())
			{
				if (APS3GameStateBase* GS = World->GetGameState<APS3GameStateBase>())
				{
					GS->OnEscapeDoorOpened.AddDynamic(this, &ADoor::OnOpenDoor);
					if (GS->IsEscapeDoorOpened())
					{
						OnOpenDoor(true);
					}
				}
			}
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
				UE_LOG(LogTemp, Warning, TEXT("[Door] %s (DoorID: %d) -> OverlapSwitch (%s) ID 연동 완료"), 
					*GetName(), DoorID, *Actor->GetName());
			}
		}

		// 2. InteractionSwitchComponent 검사
		if (UInteractionSwitchComponent* InteractComp = Actor->FindComponentByClass<UInteractionSwitchComponent>())
		{
			if (InteractComp->SwitchID == DoorID)
			{
				InteractComp->OnSwitchActivatedChanged.AddUObject(this, &ADoor::OnLinkedSwitchStateChanged);
				LinkedInteractionSwitches.Add(InteractComp);
				UE_LOG(LogTemp, Warning, TEXT("[Door] %s (DoorID: %d) -> InteractionSwitch (%s) ID 연동 완료"), 
					*GetName(), DoorID, *Actor->GetName());
			}
		}
	}

	// 바인딩 직후 초기 상태 평가
	EvaluateDoorState();
}

void ADoor::EvaluateDoorState()
{
	if (!HasAuthority()) return;

	EvaluateDoorState();
}

void ADoor::OnLinkedSwitchStateChanged(bool bIsActivated)
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
		UE_LOG(LogTemp, Warning, TEXT("[Door] DoorID %d 문 상태 변경: bIsOpen = %s (활성화: %d / 전체: %d)"),
			DoorID, bIsOpen ? TEXT("True") : TEXT("False"), ActiveCount, TotalLinkedSwitches);
	}
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 열림/닫힘 목표 위치 계산
	FVector TargetPos = bIsOpen ? (InitialRelativeLocation + TargetRelativeLocation) : InitialRelativeLocation;
	FVector CurrentPos = DoorMesh->GetRelativeLocation();

	// Smooth Interp 문 이동 (열림 & 닫힘 모두 작동)
	if (!CurrentPos.Equals(TargetPos, 1.0f))
	{
		FVector NewPos = FMath::VInterpTo(CurrentPos, TargetPos, DeltaTime, OpenSpeed);
		DoorMesh->SetRelativeLocation(NewPos);
	}
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

	//test dnjsqls
	UE_LOG(LogTemp, Warning, TEXT("[Door] 서버: OnOpenDoor 호출됨! bIsOpen = %s"), bIsOpen ? TEXT("True") : TEXT("False"));
}

void ADoor::OnRep_bIsOpen()
{
	//test dnjsqls
	UE_LOG(LogTemp, Log, TEXT("[Door] 클라이언트: OnRep_bIsOpen 호출됨 (클라이언트 동기화 완료)"));
}
