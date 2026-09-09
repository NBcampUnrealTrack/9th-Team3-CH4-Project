#include "Object/Jeoul.h"
#include "Dumbbell.h"
#include "Camera/CameraComponent.h"
#include "Component/InteractionSwitchComponent.h"
#include "Components/BoxComponent.h"
#include "Core/GameMode/PS3GameModeS4.h"
#include "Core/GameState/PS3GameStateBase.h"
#include "Core/GameState/PS3GameStateS4.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Player/Character/PS3PlayerCharacter.h"
#include "Player/Controller/PS3PlayerController.h"

AJeoul::AJeoul()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	JeoulBaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JeoulBaseMesh"));
	JeoulBaseMesh->SetupAttachment(RootComponent);
	JeoulBaseMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	JeoulBaseMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	
	BeamPivot = CreateDefaultSubobject<USceneComponent>(TEXT("BeamPivot"));
	BeamPivot->SetupAttachment(JeoulBaseMesh);

	JeoulBeamMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JeoulBeamMesh"));
	JeoulBeamMesh->SetupAttachment(BeamPivot);
	JeoulBeamMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	JeoulBeamMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore); // 추가!
	
	PlateTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("PlateTrigger"));
	PlateTrigger->SetupAttachment(BeamPivot);
	PlateTrigger->SetCollisionObjectType(ECC_GameTraceChannel2); // "Trigger"로 이름 붙인 채널
	PlateTrigger->SetCollisionResponseToAllChannels(ECR_Overlap);
	PlateTrigger->SetGenerateOverlapEvents(true);
	
	// 컷씬 전경 카메라 배치
	CutsceneCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CutsceneCamera"));
	CutsceneCamera->SetupAttachment(RootComponent);

	// 플레이어가 조준할 버튼 메쉬 생성 및 저울 기둥/몸체에 부착
	CheckButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CheckButtonMesh"));
	CheckButtonMesh->SetupAttachment(JeoulBaseMesh);
	CheckButtonMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	CheckButtonMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // 버튼만 Block!

	// 2. 스위치 컴포넌트 생성 및 저울 전용 설정
	InteractionSwitchComp = CreateDefaultSubobject<UInteractionSwitchComponent>(TEXT("InteractionSwitchComp"));
	InteractionSwitchComp->SetRegisterToGameMode(false); // GM 집계 제외
	
	// ★ 플레이어 정렬용 스폿 생성 (BeamPivot 하위에 부착되어 저울과 함께 기울어짐)
	Player1Spot = CreateDefaultSubobject<USceneComponent>(TEXT("Player1Spot"));
	Player1Spot->SetupAttachment(BeamPivot);

	Player2Spot = CreateDefaultSubobject<USceneComponent>(TEXT("Player2Spot"));
	Player2Spot->SetupAttachment(BeamPivot);
	
	// ★ 덤벨 정렬용 슬롯 3개 생성
	for (int32 i = 0; i < 3; ++i)
	{
		FName SpotName = *FString::Printf(TEXT("DumbbellSpot_%d"), i + 1);
		USceneComponent* DumbbellSpot = CreateDefaultSubobject<USceneComponent>(SpotName);
		DumbbellSpot->SetupAttachment(BeamPivot);
		DumbbellSpots.Add(DumbbellSpot);
	}
	
	SetupBlockingMesh(JeoulBaseMesh, ECR_Ignore);
	SetupBlockingMesh(JeoulBeamMesh, ECR_Ignore);
	SetupBlockingMesh(CheckButtonMesh, ECR_Block);
}

void AJeoul::RequestCutsceneReturn(APS3PlayerController* RequestingController)
{
	if (!HasAuthority()
		|| CurrentState == EJeoulState::Checking
		|| !IsValid(RequestingController))
	{
		return;
	}

	const bool bIsParticipant =
		CutsceneParticipants.ContainsByPredicate(
			[RequestingController](
				const TWeakObjectPtr<APS3PlayerController>& Participant)
			{
				return Participant.Get() == RequestingController;
			});

	if (!bIsParticipant) return;

	// 먼저 비워 중복 F 입력을 무시합니다.
	const TArray<TWeakObjectPtr<APS3PlayerController>> Participants =
		MoveTemp(CutsceneParticipants);

	CutsceneParticipants.Reset();

	for (const TWeakObjectPtr<APS3PlayerController>& Participant : Participants)
	{
		if (APS3PlayerController* PlayerController = Participant.Get())
		{
			PlayerController->Client_EndJeoulCutscene(this);
		}
	}
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

bool AJeoul::HasBothPlayersOnPlate() const
{
	if (!PlateTrigger) return false;

	TArray<AActor*> OverlappingActors;
	PlateTrigger->GetOverlappingActors(OverlappingActors);

	int32 PlayerCount = 0;
	for (AActor* Actor : OverlappingActors)
	{
		if (Cast<ACharacter>(Actor))
		{
			PlayerCount++;
		}
	}

	// 플레이어 2명이 모두 오버랩 중인지 반환
	return PlayerCount >= 2;
}

void AJeoul::SetupBlockingMesh(UStaticMeshComponent* Mesh, ECollisionResponse VisibilityResponse)
{
	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, VisibilityResponse);
}

void AJeoul::OnCheckButtonPressed(bool bActivated)
{
	if (bActivated && HasAuthority() && CurrentState == EJeoulState::Idle)
	{
		Server_CheckBalance();
	}
}

void AJeoul::AlignPlayersAndDumbbells()
{
	if (!HasAuthority() || !PlateTrigger) return;

	TArray<AActor*> OverlappingActors;
	PlateTrigger->GetOverlappingActors(OverlappingActors);

	int32 DumbbellIndex = 0;

	for (AActor* Actor : OverlappingActors)
	{
		if (!Actor) continue;

		// 1. 저울판 위 플레이어 위치 및 정면 바라보기 정렬
		if (ACharacter* Character = Cast<ACharacter>(Actor))
		{
			APlayerController* PC = Cast<APlayerController>(Character->GetController());
			int32 PlayerIndex = (PC && PC->PlayerState) ? PC->PlayerState->GetPlayerId() : 0;

			// P1(0번)과 P2(1번) 각각의 지정 위치 스폿 선택
			USceneComponent* TargetSpot = (PlayerIndex == 0) ? Player1Spot : Player2Spot;
			if (TargetSpot)
			{
				FVector TargetLocation = TargetSpot->GetComponentLocation();
				FRotator TargetRotation = TargetSpot->GetComponentRotation();

				// 캐릭터 위치 및 회전 강제 이동
				Character->TeleportTo(TargetLocation, TargetRotation);

				// 컨트롤러 시선 각도도 정면 스폿 회전값으로 맞춤
				if (PC)
				{
					PC->SetControlRotation(TargetRotation);
				}
			}
		}
		// 2. 저울판 위 덤벨 슬롯 정렬 (들려있지 않은 덤벨만)
		else if (ADumbbell* Dumbbell = Cast<ADumbbell>(Actor))
		{
			if (!Dumbbell->IsHeld() && DumbbellSpots.IsValidIndex(DumbbellIndex))
			{
				USceneComponent* SlotSpot = DumbbellSpots[DumbbellIndex];
				if (SlotSpot)
				{
					Dumbbell->AttachToComponent(
						BeamPivot,
						FAttachmentTransformRules::SnapToTargetNotIncludingScale
					);

					// 슬롯 위치 및 회전에 딱 맞춰 정렬
					Dumbbell->SetActorRelativeLocation(SlotSpot->GetRelativeLocation());
					Dumbbell->SetActorRelativeRotation(SlotSpot->GetRelativeRotation());
				}
				DumbbellIndex++;
			}
		}
	}
}

float AJeoul::CalculateWeightOnPlate(UBoxComponent* InPlateTrigger)
{
	if (!InPlateTrigger) return 0.0f;

	TArray<AActor*> OverlappingActors;
	InPlateTrigger->GetOverlappingActors(OverlappingActors);

	float TotalWeight = 0.0f;

	// 중복 집계 방지용 (바닥에 놓인 덤벨과 들고 있는 덤벨이 이중 계산되지 않도록)
	TSet<ADumbbell*> CountedDumbbells;
	
	for (AActor* Actor : OverlappingActors)
	{
		if (!Actor) continue;

		/// 바닥에 직접 올려진 Dumbbell인 경우
		if (ADumbbell* Dumbbell = Cast<ADumbbell>(Actor))
		{
			if (!CountedDumbbells.Contains(Dumbbell))
			{
				TotalWeight += Dumbbell->GetWeight();
				CountedDumbbells.Add(Dumbbell);

				// 서버 권한에서 덤벨을 BeamPivot에 부착하여 저울대가 기울어질 때 함께 이동
				if (HasAuthority())
				{
					Dumbbell->AttachToComponent(
						BeamPivot,
						FAttachmentTransformRules::KeepWorldTransform
					);
				}
			}
		}
		//플레이어가 저울판 위에 올라와 있고, 덤벨을 들고 있는 경우
		else if (APS3PlayerCharacter* PlayerChar = Cast<APS3PlayerCharacter>(Actor))
		{
			if (ADumbbell* HeldDumbbell = PlayerChar->GetHeldDumbbell()) // 또는 HeldDumbbell 멤버변수 접근
			{
				if (!CountedDumbbells.Contains(HeldDumbbell))
				{
					TotalWeight += HeldDumbbell->GetWeight();
					CountedDumbbells.Add(HeldDumbbell);
				}
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
	if (CurrentState != EJeoulState::Idle) return;
	
	CutsceneParticipants.RemoveAll(
		[](const TWeakObjectPtr<APS3PlayerController>& Participant)
		{
			return !Participant.IsValid();
		});

	if (!CutsceneParticipants.IsEmpty()) return;
	
	if (!HasBothPlayersOnPlate())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Jeoul] 체크 실패: 플레이어 2명이 모두 저울판 위에 올라와 있지 않습니다."));
		
		// 스위치가 눌린 상태로 굳지 않도록 스위치 상태 리셋
		if (InteractionSwitchComp)
		{
			InteractionSwitchComp->ResetSwitch();
		}
		return;
	}
	
	CurrentState = EJeoulState::Checking;
	
	// ★ 컷씬 및 균형 확인 시작 전 플레이어와 덤벨을 지정 스폿 위치로 즉시 정렬!
	AlignPlayersAndDumbbells();
	
	// ★ 2. 서버에서 저울판 위 플레이어의 소유 클라이언트에 카메라 전환 요청
	TArray<AActor*> PlayersOnPlate;
	PlateTrigger->GetOverlappingActors(
		PlayersOnPlate,
		APS3PlayerCharacter::StaticClass());
	
	for (AActor* PlayerActor : PlayersOnPlate)
	{
		APS3PlayerCharacter* PlayerCharacter =
			Cast<APS3PlayerCharacter>(PlayerActor);

		if (!IsValid(PlayerCharacter)) continue;

		if (APS3PlayerController* PlayerController =
			Cast<APS3PlayerController>(PlayerCharacter->GetController()))
		{
			CutsceneParticipants.AddUnique(
				TWeakObjectPtr<APS3PlayerController>(PlayerController));

			PlayerController->Client_BeginJeoulCutscene(this);
		}
	}
	
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
				InteractionSwitchComp->SetLocked(true);
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
