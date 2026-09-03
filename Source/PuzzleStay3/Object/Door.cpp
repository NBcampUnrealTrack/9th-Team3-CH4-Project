#include "Object/Door.h"

#include "Core/GameMode/PS3GameModeBase.h"
#include "Core/GameState/PS3GameStateBase.h"
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
		if (APS3GameStateBase* GS = World->GetGameState<APS3GameStateBase>())
		{
			// AddDynamic 또는 AddUniqueDynamic 사용
			GS->OnEscapeDoorOpened.AddDynamic(this, &ADoor::OnOpenDoor);
			UE_LOG(LogTemp, Warning, TEXT("[Door] GameState OnEscapeDoorOpened 델리게이트 바인딩 완료!"));

			// 만약 이미 문이 열린 상태로 늦게 진입/초기화된 경우를 대비한 초기값 반영
			if (GS->IsEscapeDoorOpened())
			{
				OnOpenDoor(true);
			}
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

