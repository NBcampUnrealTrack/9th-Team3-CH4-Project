#include "Object/Door.h"

#include "Core/GameMode/PS3GameModeBase.h"
#include "Core/GameState/PS3GameStateBase.h"
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
	
	InitialRelativeLocation = DoorMesh->GetRelativeLocation();
	
	// GameStateBase의 델리게이트 구독 (서버 및 클라이언트 모두 바인딩 가능)
	if (UWorld* World = GetWorld())
	{
		switch (DoorType)
		{
			// 1. 전 스테이지 공통 최종 탈출문
		case EDoorType::StageAllFinalDoor:
			if (APS3GameStateBase* GS = World->GetGameState<APS3GameStateBase>())
			{
				GS->OnEscapeDoorOpened.AddDynamic(this, &ADoor::OnOpenDoor);
				UE_LOG(LogTemp, Warning, TEXT("[Door] StageAllFinalDoor 델리게이트 바인딩 완료!"));

				if (GS->IsEscapeDoorOpened())
				{
					OnOpenDoor(true);
				}
			}
			break;

			// 2. Stage 1 일반문
		case EDoorType::Stage1NormalDoor:
			// TODO: Stage 1 전용 GameState(예: APS3GameStateS1)의 문 열림 델리게이트 연동
			/*
			if (APS3GameStateS1* GS = World->GetGameState<APS3GameStateS1>())
			{
				GS->OnStage1DoorOpenedChanged.AddDynamic(this, &ADoor::OnOpenDoor);
				if (GS->IsStage1DoorOpened()) OnOpenDoor(true);
			}
			*/
			UE_LOG(LogTemp, Warning, TEXT("[Door] Stage1NormalDoor 세팅됨 (GameStateS1 연동 준비 완료)"));
			break;

			// 3. Stage 4 첫 번째 문 (저울 기믹 완료 문)
		case EDoorType::Stage4FirstDoor:
			if (APS3GameStateS4* GS = World->GetGameState<APS3GameStateS4>())
			{
				GS->OnStage4FirstDoorOpenedChanged.AddDynamic(this, &ADoor::OnOpenDoor);
				UE_LOG(LogTemp, Warning, TEXT("[Door] Stage4FirstDoor 델리게이트 바인딩 완료!"));

				if (GS->IsStage4FirstDoorOpened())
				{
					OnOpenDoor(true);
				}
			}
			break;

			// 4. Stage 5 일반문
		case EDoorType::Stage5NormalDoor:
			// TODO: Stage 5 전용 GameState(예: APS3GameStateS5)의 문 열림 델리게이트 연동
			/*
			if (APS3GameStateS5* GS = World->GetGameState<APS3GameStateS5>())
			{
				GS->OnStage5DoorOpenedChanged.AddDynamic(this, &ADoor::OnOpenDoor);
				if (GS->IsStage5DoorOpened()) OnOpenDoor(true);
			}
			*/
			UE_LOG(LogTemp, Warning, TEXT("[Door] Stage5NormalDoor 세팅됨 (GameStateS5 연동 준비 완료)"));
			break;

		default:
			break;
		}
	}
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsOpen)
	{
		FVector TargetPos = InitialRelativeLocation + TargetRelativeLocation;
		FVector CurrentPos = DoorMesh->GetRelativeLocation();

		if (!CurrentPos.Equals(TargetPos, 1.0f))
		{
			FVector NewPos = FMath::VInterpTo(CurrentPos, TargetPos, DeltaTime, OpenSpeed);
			DoorMesh->SetRelativeLocation(NewPos);
		}
	}
}

void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADoor, bIsOpen);
}

void ADoor::OnOpenDoor(bool bOpened)
{
	bIsOpen = bOpened;
	OnRep_bIsOpen();
	
	//test dnjsqls
	UE_LOG(LogTemp, Warning, TEXT("[Door] OnOpenDoor 호출됨! bIsOpen = %s"), bIsOpen ? TEXT("True") : TEXT("False"));
	if (GEngine && bIsOpen)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Door: Opening!"));
	}
}

void ADoor::OnRep_bIsOpen()
{
	// 클라이언트 및 서버 공통 반응 (필요시 문 열림 사운드/이펙트 재생)
	//test dnjsqls
	UE_LOG(LogTemp, Log, TEXT("[Door] OnRep_bIsOpen 호출됨 (클라이언트 동기화 완료)"));
}

