#include "Object/Jeoul.h"

#include "Dumbbell.h"
#include "Component/InteractionSwitchComponent.h"
#include "Components/BoxComponent.h"
#include "Core/GameMode/PS3GameModeBase.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

AJeoul::AJeoul()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	JeoulBaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JeoulBaseMesh"));
	JeoulBaseMesh->SetupAttachment(RootComponent);

	JeoulBeamMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JeoulBeamMesh"));
	JeoulBeamMesh->SetupAttachment(JeoulBaseMesh);

	LeftPlateTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftPlateTrigger"));
	LeftPlateTrigger->SetupAttachment(JeoulBeamMesh);

	RightPlateTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("RightPlateTrigger"));
	RightPlateTrigger->SetupAttachment(JeoulBeamMesh);

	InteractionSwitchComp = CreateDefaultSubobject<UInteractionSwitchComponent>(TEXT("InteractionSwitchComp"));
}

void AJeoul::BeginPlay()
{
	Super::BeginPlay();
	
	// 저울대의 초기 회전값(수평 상태) 저장
	InitialBeamRotation = JeoulBeamMesh->GetRelativeRotation();

	if (HasAuthority())
	{
		InteractionSwitchComp->OnSwitchActivatedChanged.AddUObject(this, &AJeoul::OnCheckButtonPressed);
	}
}

void AJeoul::OnCheckButtonPressed(bool bActivated)
{
	if (HasAuthority())
	{
		Server_CheckBalance();
	}
}

float AJeoul::CalculateWeightOnPlate(UBoxComponent* PlateTrigger) const
{
	if (!PlateTrigger) return 0.0f;

	TArray<AActor*> OverlappingActors;
	PlateTrigger->GetOverlappingActors(OverlappingActors);

	float TotalWeight = 0.0f;

	for (AActor* Actor : OverlappingActors)
	{
		if (!Actor) continue;

		// 1. Dumbbell 무게 합산
		if (Actor->IsA<ADumbbell>())
		{
			TotalWeight += 1.0f; 
		}
		// 2. 플레이어 무게 합산
		else if (ACharacter* Character = Cast<ACharacter>(Actor))
		{
			APlayerController* PC = Cast<APlayerController>(Character->GetController());
			if (PC)
			{
				int32 PlayerIndex = UGameplayStatics::GetPlayerControllerID(PC);
				TotalWeight += (PlayerIndex == 0) ? PlayerAWeight : PlayerBWeight;
			}
		}
	}
	return TotalWeight;
}

void AJeoul::Server_CheckBalance_Implementation()
{
	float LeftWeight = CalculateWeightOnPlate(LeftPlateTrigger);
	float RightWeight = CalculateWeightOnPlate(RightPlateTrigger);
	
	UE_LOG(LogTemp, Log, TEXT("[Jeoul] 좌측 무게: %f | 우측 무게: %f"), LeftWeight, RightWeight);
	
	// 좌/우 무게 균형 판정 (오차 범위 극소)
	if (FMath::IsNearlyEqual(LeftWeight, RightWeight, KINDA_SMALL_NUMBER) && LeftWeight > 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Jeoul] 수평 평형 성공! 1차 문 개방"));
		
		if (APS3GameModeBase* GM = Cast<APS3GameModeBase>(GetWorld()->GetAuthGameMode()))
		{
			GM->OnEscapeDoorOpened.Broadcast();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Jeoul] 수평 평형 실패! 컷씬 연출 후 저울 기울기를 원위치로 리셋합니다."));
		ResetJeoulRotation();
	}
}

void AJeoul::ResetJeoulRotation()
{
	// 덤벨은 저울 위에 유지되고, 저울대의 회전(기울기)만 초기 위치로 복구합니다.
	JeoulBeamMesh->SetRelativeRotation(InitialBeamRotation);
}

