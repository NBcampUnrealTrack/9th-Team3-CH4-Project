#include "Object/Jeoul.h"
#include "Dumbbell.h"
#include "Camera/CameraComponent.h"
#include "Component/InteractionSwitchComponent.h"
#include "Components/BoxComponent.h"
#include "Core/GameMode/PS3GameModeS4.h"
#include "Core/GameState/PS3GameStateBase.h"
#include "Core/GameState/PS3GameStateS4.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
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
	JeoulBaseMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	PlateTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("PlateTrigger"));
	PlateTrigger->SetupAttachment(BeamPivot);
	PlateTrigger->SetCollisionObjectType(ECC_GameTraceChannel2); // "Trigger"로 이름 붙인 채널
	PlateTrigger->SetCollisionResponseToAllChannels(ECR_Overlap);
	PlateTrigger->SetGenerateOverlapEvents(true);

	CutsceneCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CutsceneCamera"));
	CutsceneCamera->SetupAttachment(RootComponent);

	CheckButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CheckButtonMesh"));
	CheckButtonMesh->SetupAttachment(JeoulBaseMesh);
	CheckButtonMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	CheckButtonMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // 버튼만 Block!

	InteractionSwitchComp = CreateDefaultSubobject<UInteractionSwitchComponent>(TEXT("InteractionSwitchComp"));
	InteractionSwitchComp->SetRegisterToGameMode(false); // GM 집계 제외

	Player1Spot = CreateDefaultSubobject<USceneComponent>(TEXT("Player1Spot"));
	Player1Spot->SetupAttachment(BeamPivot);
	Player2Spot = CreateDefaultSubobject<USceneComponent>(TEXT("Player2Spot"));
	Player2Spot->SetupAttachment(BeamPivot);

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

void AJeoul::BeginPlay()
{
	Super::BeginPlay();

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

	return PlayerCount >= 2;
}

void AJeoul::SetupBlockingMesh(UStaticMeshComponent* Mesh, ECollisionResponse VisibilityResponse)
{
	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, VisibilityResponse);
}

void AJeoul::OnCheckButtonPressed(bool bActivated)
{
	if (!HasAuthority()) return;

	if (!bActivated) return;
	
	if (CurrentState == EJeoulState::Idle)
	{
		Server_CheckBalance();
	}
	else if (CurrentState == EJeoulState::Resolved || CurrentState == EJeoulState::Checking)
	{
		if (APS3PlayerCharacter* InteractingChar = Cast<APS3PlayerCharacter>(
			InteractionSwitchComp->GetInteractingActor()))
		{
			if (APS3PlayerController* PC = Cast<APS3PlayerController>(InteractingChar->GetController()))
			{
				RequestCutsceneReturn(PC);
			}
		}
	}
}

void AJeoul::Multicast_RestorePlayerCharacter_Implementation(APS3PlayerCharacter* TargetCharacter)
{
	if (!IsValid(TargetCharacter)) return;

	TargetCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// [수정: 2번 방식] MovementMode 제어 코드 제거 -> 캐릭터 입력 복구 플래그만 true 설정
	TargetCharacter->SetCanUseFieldControls(true);
}

void AJeoul::Multicast_AlignPlayerCharacter_Implementation(APS3PlayerCharacter* TargetCharacter, FVector TargetLocation,
                                                           FRotator TargetRotation, USceneComponent* AttachTarget)
{
	if (!IsValid(TargetCharacter)) return;

	TargetCharacter->SetCanUseFieldControls(false);
	
	if (UCharacterMovementComponent* MovementComp = TargetCharacter->GetCharacterMovement())
	{
		MovementComp->StopMovementImmediately();
	}

	TargetCharacter->SetActorLocationAndRotation(TargetLocation, TargetRotation, false, nullptr,
												 ETeleportType::TeleportPhysics);

	if (AttachTarget)
	{
		TargetCharacter->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepWorldTransform);
	}

	if (AController* Controller = TargetCharacter->GetController())
	{
		Controller->SetControlRotation(TargetRotation);
	}
}

void AJeoul::AlignPlayersAndDumbbells()
{
	if (!HasAuthority() || !PlateTrigger) return;

	TArray<AActor*> OverlappingActors;
	PlateTrigger->GetOverlappingActors(OverlappingActors);

	int32 DumbbellIndex = 0;
	int32 PlayerSpotIndex = 0;

	for (AActor* Actor : OverlappingActors)
	{
		if (!Actor) continue;

		if (ACharacter* Character = Cast<ACharacter>(Actor))
		{
			USceneComponent* TargetSpot = (PlayerSpotIndex == 0) ? Player1Spot : Player2Spot;
			PlayerSpotIndex++;

			if (TargetSpot)
			{
				FVector TargetLocation = TargetSpot->GetComponentLocation();
				FRotator TargetRotation = TargetSpot->GetComponentRotation();

				if (UCapsuleComponent* Capsule = Character->GetCapsuleComponent())
				{
					TargetLocation.Z += Capsule->GetScaledCapsuleHalfHeight();
				}
				Multicast_AlignPlayerCharacter(Cast<APS3PlayerCharacter>(Character), TargetLocation, TargetRotation,
				                               BeamPivot);
			}
		}
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
					Dumbbell->SetActorRelativeLocation(SlotSpot->GetRelativeLocation());
					Dumbbell->SetActorRelativeRotation(SlotSpot->GetRelativeRotation());
				}
				DumbbellIndex++;
			}
		}
	}
}

void AJeoul::RequestCutsceneReturn(APS3PlayerController* RequestingController)
{
	if (!HasAuthority() || !IsValid(RequestingController))
	{
		return;
	}

	const TArray<TWeakObjectPtr<APS3PlayerController>> Participants = MoveTemp(CutsceneParticipants);
	CutsceneParticipants.Reset();

	for (const TWeakObjectPtr<APS3PlayerController>& Participant : Participants)
	{
		if (APS3PlayerController* PlayerController = Participant.Get())
		{
			if (APS3PlayerCharacter* TargetChar = Cast<APS3PlayerCharacter>(PlayerController->GetPawn()))
			{
				// 전 클라이언트에서 부착 해제 및 이동 기능 동시 복구
				Multicast_RestorePlayerCharacter(TargetChar);
			}

			PlayerController->Client_EndJeoulCutscene(this);
		}
	}
}

float AJeoul::CalculateWeightOnPlate(UBoxComponent* InPlateTrigger)
{
	if (!InPlateTrigger) return 0.0f;

	TArray<AActor*> OverlappingActors;
	InPlateTrigger->GetOverlappingActors(OverlappingActors);

	float TotalWeight = 0.0f;
	TSet<ADumbbell*> CountedDumbbells;

	for (AActor* Actor : OverlappingActors)
	{
		if (!Actor) continue;

		if (ADumbbell* Dumbbell = Cast<ADumbbell>(Actor))
		{
			if (!CountedDumbbells.Contains(Dumbbell))
			{
				TotalWeight += Dumbbell->GetWeight();
				CountedDumbbells.Add(Dumbbell);

				if (HasAuthority())
				{
					Dumbbell->AttachToComponent(
						BeamPivot,
						FAttachmentTransformRules::KeepWorldTransform
					);
				}
			}
		}
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

		if (InteractionSwitchComp)
		{
			InteractionSwitchComp->ResetSwitch();
		}
		return;
	}

	CurrentState = EJeoulState::Checking;

	AlignPlayersAndDumbbells();

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

	const float TotalWeight = CalculateWeightOnPlate(PlateTrigger);

	float JudgeWeight = 0.0f;
	if (APS3GameStateS4* GS = GetWorld()->GetGameState<APS3GameStateS4>())
	{
		JudgeWeight = GS->GetTargetBalancedWeight();
	}

	float WeightDifference = JudgeWeight - TotalWeight;

	float TargetRoll = FMath::Clamp(WeightDifference * TiltSensitivity, -MaxTiltAngle, MaxTiltAngle);
	TargetBeamRotation = InitialBeamRotation + FRotator(0.0f, 0.0f, TargetRoll);

	//dnjsqls
	UE_LOG(LogTemp, Warning, TEXT("[Jeoul] 무게 차이: %f, TargetRoll: %f, 현재 무게: %f, 목표 무게: %f"), WeightDifference,
	       TargetRoll, TotalWeight, JudgeWeight);

	// CutSceneTime 후 컷씬 종료 및 결과 판단 타이머
	FTimerHandle ResultTimer;
	GetWorldTimerManager().SetTimer(ResultTimer, [this, TotalWeight, JudgeWeight]()
	{
		bool bIsSuccess = FMath::IsNearlyEqual(TotalWeight, JudgeWeight, 0.01f) && TotalWeight > 0.0f;

		if (APS3GameModeS4* GM = Cast<APS3GameModeS4>(GetWorld()->GetAuthGameMode()))
		{
			GM->NotifyJeoulResult(bIsSuccess);
		}

		if (bIsSuccess)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Jeoul] 판정 성공! 저울 완료 처리"));
			CurrentState = EJeoulState::Resolved;

			Multicast_OnJeoulCheckFinished(true);

			if (InteractionSwitchComp)
			{
				InteractionSwitchComp->ResetSwitch();
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[Jeoul] 무게 불균형! 저울대 기울기 원위치 리셋"));

			TargetBeamRotation = InitialBeamRotation;

			if (InteractionSwitchComp)
			{
				InteractionSwitchComp->ResetSwitch(); // 컷씬 종료 시점에 리셋
			}

			FTimerHandle ResetTimer;
			GetWorldTimerManager().SetTimer(ResetTimer, [this]()
			{
				CurrentState = EJeoulState::Idle;
				Multicast_OnJeoulCheckFinished(false);
			}, ResetBeamTime, false);
		}
	}, CutSceneTime, false);
}