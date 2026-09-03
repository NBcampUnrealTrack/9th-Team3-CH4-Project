#include "Object/Jeoul.h"

#include "Dumbbell.h"
#include "Camera/CameraComponent.h"
#include "Component/InteractionSwitchComponent.h"
#include "Components/BoxComponent.h"
#include "Core/GameMode/PS3GameModeBase.h"
#include "Core/GameMode/PS3GameModeS4.h"
#include "Core/GameState/PS3GameStateBase.h"
#include "Core/GameState/PS3GameStateS4.h"
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

	BeamPivot = CreateDefaultSubobject<USceneComponent>(TEXT("BeamPivot"));
	BeamPivot->SetupAttachment(JeoulBaseMesh);

	JeoulBeamMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JeoulBeamMesh"));
	JeoulBeamMesh->SetupAttachment(BeamPivot);

	PlateTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("PlateTrigger"));
	PlateTrigger->SetupAttachment(BeamPivot);

	// 컷씬 전경 카메라 배치
	CutsceneCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CutsceneCamera"));
	CutsceneCamera->SetupAttachment(RootComponent);

	// 플레이어가 조준할 버튼 메쉬 생성 및 저울 기둥/몸체에 부착
	CheckButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CheckButtonMesh"));
	CheckButtonMesh->SetupAttachment(JeoulBaseMesh);

	// 라인트레이스 감지를 위해 Collision Profile을 Visibility 채널에 블록(Block)되도록 설정
	CheckButtonMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	// 2. 스위치 컴포넌트 생성 및 저울 전용 설정
	InteractionSwitchComp = CreateDefaultSubobject<UInteractionSwitchComponent>(TEXT("InteractionSwitchComp"));
	InteractionSwitchComp->SetRegisterToGameMode(false); // GM 집계 제외
}

void AJeoul::BeginPlay()
{
	Super::BeginPlay();

	// 저울대의 초기 회전값(수평 상태) 저장
	InitialBeamRotation = BeamPivot->GetRelativeRotation();
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
	FRotator CurrentRot = BeamPivot->GetRelativeRotation();
	if (!CurrentRot.Equals(TargetBeamRotation, 0.1f))
	{
		FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetBeamRotation, DeltaTime, 3.0f);
		BeamPivot->SetRelativeRotation(NewRot);
	}
}

void AJeoul::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AJeoul, TargetBeamRotation);
	DOREPLIFETIME(AJeoul, CurrentState);
}

void AJeoul::OnRep_TargetBeamRotation()
{
	// 클라이언트 측에서 TargetBeamRotation 업데이트 시 보간 애니메이션이 Tick에서 즉시 동작함
}

void AJeoul::OnCheckButtonPressed(bool bActivated)
{
	if (bActivated && HasAuthority() && CurrentState == EJeoulState::Idle)
	{
		Server_CheckBalance();
	}
}

float AJeoul::CalculateWeightOnPlate(UBoxComponent* InPlateTrigger)
{
	if (!InPlateTrigger) return 0.0f;

	TArray<AActor*> OverlappingActors;
	InPlateTrigger->GetOverlappingActors(OverlappingActors);

	float TotalWeight = 0.0f;

	for (AActor* Actor : OverlappingActors)
	{
		if (!Actor) continue;

		// 1. Dumbbell 무게 합산 (안고 있는 상태여도 포함)
		if (ADumbbell* Dumbbell = Cast<ADumbbell>(Actor))
		{
			TotalWeight += Dumbbell->GetWeight();

			// 서버 권한에서 덤벨을 BeamPivot에 부착하여 저울대가 기울어질 때 함께 이동
			if (HasAuthority())
			{
				Dumbbell->AttachToComponent(
					BeamPivot,
					FAttachmentTransformRules::KeepWorldTransform
				);
			}
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

	// 스위치를 누른 순간의 무게 스냅샷 측정
	const float TotalWeight = CalculateWeightOnPlate(PlateTrigger);

	// GameStateS4에서 이번 스테이지의 목표 정답 무게 가져오기
	float JudgeWeight = 0.0f;
	if (APS3GameStateS4* GS = GetWorld()->GetGameState<APS3GameStateS4>())
	{
		JudgeWeight = GS->GetTargetBalancedWeight();
	}

	//무게 차이 계산
	float WeightDifference = JudgeWeight - TotalWeight;

	// 무게 차이에 따른 기울기 목표 각도 산출 (Pitch 또는 Roll 축 제어)
	float TargetRoll = FMath::Clamp(WeightDifference * TiltSensitivity, -MaxTiltAngle, MaxTiltAngle);
	TargetBeamRotation = InitialBeamRotation + FRotator(0.0f, 0.0f, TargetRoll);

	//dnjsqls
	UE_LOG(LogTemp, Warning, TEXT("[Jeoul] 무게 차이: %f, TargetRoll: %f, 현재 무게: %f, 목표 무게: %f"), WeightDifference,
	       TargetRoll, TotalWeight, JudgeWeight);

	// CutSceneTime 후 컷씬 종료 및 결과 판단 타이머
	FTimerHandle ResultTimer;
	GetWorldTimerManager().SetTimer(ResultTimer, [this, TotalWeight, JudgeWeight]()
	{
		// 수평(동일 무게) 판정: 오차 허용 범위 0.01f 적용
		bool bIsSuccess = FMath::IsNearlyEqual(TotalWeight, JudgeWeight, 0.01f) && TotalWeight > 0.0f;

		//GameModeS4에 판정 결과 통보 (GameMode가 문 개방 로직을 구동함)
		if (APS3GameModeS4* GM = Cast<APS3GameModeS4>(GetWorld()->GetAuthGameMode()))
		{
			GM->NotifyJeoulResult(bIsSuccess);
		}

		if (bIsSuccess)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Jeoul] 판정 성공! 저울 완료 처리"));
			CurrentState = EJeoulState::Resolved;

			// 성공 시 모든 클라이언트에 알림
			Multicast_OnJeoulCheckFinished(true);

			if (InteractionSwitchComp)
			{
				InteractionSwitchComp->ResetSwitch(); // 컷씬 종료 시점에 리셋
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[Jeoul] 무게 불균형! 저울대 기울기 원위치 리셋"));

			// 실패 시 저울대 각도를 다시 수평(초기 회전)으로 리셋
			TargetBeamRotation = InitialBeamRotation;

			if (InteractionSwitchComp)
			{
				InteractionSwitchComp->ResetSwitch(); // 컷씬 종료 시점에 리셋
			}

			// 원위치로 돌아가는 연출 시간을 위해 ResetBeamTime 후 카메라 복구 요청
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
