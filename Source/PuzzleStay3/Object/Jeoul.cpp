#include "Object/Jeoul.h"

#include "Dumbbell.h"
#include "Camera/CameraComponent.h"
#include "Component/InteractionSwitchComponent.h"
#include "Components/BoxComponent.h"
#include "Core/GameMode/PS3GameModeBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

AJeoul::AJeoul()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	JeoulBaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JeoulBaseMesh"));
	JeoulBaseMesh->SetupAttachment(RootComponent);

	JeoulBeamMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JeoulBeamMesh"));
	JeoulBeamMesh->SetupAttachment(JeoulBaseMesh);

	PlateTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("PlateTrigger"));
	PlateTrigger->SetupAttachment(JeoulBeamMesh);

	// 컷씬 전경 카메라 배치
	CutsceneCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CutsceneCamera"));
	CutsceneCamera->SetupAttachment(RootComponent);

	InteractionSwitchComp = CreateDefaultSubobject<UInteractionSwitchComponent>(TEXT("InteractionSwitchComp"));

	// 저울에 달린 스위치는 GameMode 글로벌 스위치 카운트에서 제외!
	InteractionSwitchComp->SetRegisterToGameMode(false);
}

void AJeoul::BeginPlay()
{
	Super::BeginPlay();

	// 저울대의 초기 회전값(수평 상태) 저장
	InitialBeamRotation = JeoulBeamMesh->GetRelativeRotation();
	TargetBeamRotation = InitialBeamRotation;

	if (HasAuthority())
	{
		InteractionSwitchComp->OnSwitchActivatedChanged.AddUObject(this, &AJeoul::OnCheckButtonPressed);
	}
}

void AJeoul::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 목표 회전각(기울기)으로 부드럽게 보간 연출
	FRotator CurrentRot = JeoulBeamMesh->GetRelativeRotation();
	if (!CurrentRot.Equals(TargetBeamRotation, 0.1f))
	{
		FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetBeamRotation, DeltaTime, 3.0f);
		JeoulBeamMesh->SetRelativeRotation(NewRot);
	}
}

void AJeoul::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AJeoul, TargetBeamRotation);
}

void AJeoul::OnCheckButtonPressed(bool bActivated)
{
	if (bActivated && HasAuthority() && CurrentState == EJeoulState::Idle)
	{
		Server_CheckBalance();
	}
}

float AJeoul::CalculateWeightOnPlate(UBoxComponent* InPlateTrigger) const
{
	if (!InPlateTrigger) return 0.0f;

	TArray<AActor*> OverlappingActors;
	InPlateTrigger->GetOverlappingActors(OverlappingActors);

	float TotalWeight = 0.0f;

	for (AActor* Actor : OverlappingActors)
	{
		if (!Actor) continue;

		// 1. Dumbbell 무게 합산
		if (ADumbbell* Dumbbell = Cast<ADumbbell>(Actor))
		{
			TotalWeight += Dumbbell->GetWeight();
		}
		// 2. 플레이어 무게 합산
		else if (ACharacter* Character = Cast<ACharacter>(Actor))
		{
			if (APlayerState* PS = Character->GetPlayerState())
			{
				int32 PlayerIndex = PS->GetPlayerId();
				TotalWeight += (PlayerIndex == 0) ? PlayerAWeight : PlayerBWeight;
			}
		}
	}
	return TotalWeight;
}

void AJeoul::Multicast_OnJeoulCheckStarted_Implementation()
{
	// 연출 시작 방송 (추후 GameMode가 수신 시 PlayerController 시점 전환 가능)
	OnJeoulCheckStarted.Broadcast();
}

void AJeoul::Multicast_OnJeoulCheckFinished_Implementation(bool bIsSuccess)
{
	OnJeoulCheckFinished.Broadcast(bIsSuccess);
}

void AJeoul::Server_CheckBalance_Implementation()
{
	CurrentState = EJeoulState::Checking;

	// 서버에서만 Broadcast하지 않고, 모든 클라이언트로 Multicast 호출
	Multicast_OnJeoulCheckStarted();

	float TotalWeight = CalculateWeightOnPlate(PlateTrigger);

	// GameMode에서 이번 스테이지/저울의 목표 정답 무게 가져오기
	float JudgeWeight = 0.0f;
	if (APS3GameModeBase* GM = Cast<APS3GameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		// GameMode에 선언된 TargetBalancedWeight (또는 정답 무게 Getter) 참조
		JudgeWeight = 3.f;
		//JudgeWeight = GM->GetTargetBalancedWeight(); 
	}

	//무게 차이 계산
	float WeightDifference = JudgeWeight - TotalWeight;

	// 무게 차이에 따른 기울기 목표 각도 산출 (Pitch 또는 Roll 축 제어)
	float TargetRoll = FMath::Clamp(WeightDifference * TiltSensitivity, -MaxTiltAngle, MaxTiltAngle);
	TargetBeamRotation = InitialBeamRotation + FRotator(0.0f, 0.0f, TargetRoll);

	// 3초 후 컷씬 종료 및 결과 판단 타이머
	FTimerHandle ResultTimer;
	GetWorldTimerManager().SetTimer(ResultTimer, [this, TotalWeight, JudgeWeight]()
	{
		// 수평(동일 무게) 판정
		bool bIsSuccess = FMath::IsNearlyEqual(TotalWeight, JudgeWeight, KINDA_SMALL_NUMBER) && TotalWeight > 0.0f;

		if (bIsSuccess)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Jeoul] 수평 완벽! 1차 문 개방"));
			CurrentState = EJeoulState::Resolved;

			if (APS3GameModeBase* GM = Cast<APS3GameModeBase>(GetWorld()->GetAuthGameMode()))
			{
				GM->OnEscapeDoorOpened.Broadcast();
			}
			// 성공 시 모든 클라이언트에 알림
			Multicast_OnJeoulCheckFinished(true);
			
			if (InteractionSwitchComp)
			{
				InteractionSwitchComp->ResetSwitch();   // 컷씬 종료 시점에 리셋
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[Jeoul] 무게 불균형! 저울대 기울기 원위치 리셋"));

			// 실패 시 저울대 각도를 다시 수평(초기 회전)으로 리셋
			TargetBeamRotation = InitialBeamRotation;

			if (InteractionSwitchComp)
			{
				InteractionSwitchComp->ResetSwitch();   // 컷씬 종료 시점에 리셋
			}
			
			// 원위치로 돌아가는 연출 시간을 위해 1.5초 후 카메라 복구 요청
			FTimerHandle ResetTimer;
			GetWorldTimerManager().SetTimer(ResetTimer, [this]()
			{
				CurrentState = EJeoulState::Idle;
				// 실패 후 리셋 완료 시 모든 클라이언트에 알림
				Multicast_OnJeoulCheckFinished(false);
			}, ResetBeamTime, false);
		}
	}, CutSceneTime, false);
}
