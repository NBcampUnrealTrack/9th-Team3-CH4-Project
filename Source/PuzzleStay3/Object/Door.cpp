#include "Object/Door.h"

#include "Core/GameMode/PS3GameModeBase.h"
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
	
	// 서버 권한을 가진 GameMode의 델리게이트 구독
	if (HasAuthority())
	{
		if (UWorld* World = GetWorld())
		{
			if (APS3GameModeBase* GM = Cast<APS3GameModeBase>(World->GetAuthGameMode()))
			{
				GM->OnEscapeDoorOpened.AddUObject(this, &ADoor::OnOpenDoor);
				//test dnjsqls
				UE_LOG(LogTemp, Warning, TEXT("[Door] GameMode OnEscapeDoorOpened 델리게이트 바인딩 완료!"));
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

void ADoor::OnOpenDoor()
{
	if (!HasAuthority()) return;

	bIsOpen = true;
	OnRep_bIsOpen();
	
	//test dnjsqls
	UE_LOG(LogTemp, Warning, TEXT("[Door] OnOpenDoor 호출됨! 문이 열립니다."));
	if (GEngine)
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

