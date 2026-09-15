#include "Object/Jeoul.h"
#include "Dumbbell.h"
#include "Camera/CameraComponent.h"
#include "Component/InteractionSwitchComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Core/GameMode/PS3GameModeS4.h"
#include "Core/GameState/PS3GameStateBase.h"
#include "Core/GameState/PS3GameStateS4.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/Character/PS3PlayerCharacter.h"
#include "Player/Controller/PS3PlayerController.h"

AJeoul::AJeoul()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	JeoulSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("JeoulSkeletalMesh"));
	JeoulSkeletalMesh->SetupAttachment(RootComponent);

	OverlapTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapTrigger"));
	OverlapTrigger->SetupAttachment(JeoulSkeletalMesh);
	OverlapTrigger->SetCollisionObjectType(ECC_GameTraceChannel2);
	OverlapTrigger->SetCollisionResponseToAllChannels(ECR_Overlap);
	OverlapTrigger->SetGenerateOverlapEvents(true);

	PlateTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("PlateTrigger"));
	PlateTrigger->SetupAttachment(JeoulSkeletalMesh, TEXT("ikHandle1"));
	PlateTrigger->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	PlateTrigger->SetRelativeRotation(FRotator(180.0f, 0.0f, 0.0f));

	CutsceneCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CutsceneCamera"));
	CutsceneCamera->SetupAttachment(RootComponent);

	Player1Spot = CreateDefaultSubobject<USceneComponent>(TEXT("Player1Spot"));
	Player1Spot->SetupAttachment(PlateTrigger);

	Player2Spot = CreateDefaultSubobject<USceneComponent>(TEXT("Player2Spot"));
	Player2Spot->SetupAttachment(PlateTrigger);

	// ★ 더미 메쉬 생성 및 Spot에 기본 부착 (기본값 숨김)
	DummyPlayer1Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DummyPlayer1Mesh"));
	DummyPlayer1Mesh->SetupAttachment(Player1Spot);
	DummyPlayer1Mesh->SetVisibility(false);
	DummyPlayer1Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	DummyPlayer2Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DummyPlayer2Mesh"));
	DummyPlayer2Mesh->SetupAttachment(Player2Spot);
	DummyPlayer2Mesh->SetVisibility(false);
	DummyPlayer2Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	for (int32 i = 0; i < 3; ++i)
	{
		FName SpotName = *FString::Printf(TEXT("DumbbellSpot_%d"), i + 1);
		USceneComponent* DumbbellSpot = CreateDefaultSubobject<USceneComponent>(SpotName);
		DumbbellSpot->SetupAttachment(PlateTrigger);
		DumbbellSpots.Add(DumbbellSpot);
	}

	SetupBlockingMesh(JeoulSkeletalMesh, ECR_Ignore);
}

void AJeoul::BeginPlay()
{
	Super::BeginPlay();

	// ★ 정확한 저울판 본(joint5 또는 ikHandle1) 존재 여부 검사 후 부착
	if (JeoulSkeletalMesh && JeoulSkeletalMesh->DoesSocketExist(TEXT("joint5")))
	{
		if (PlateTrigger)
		{
			PlateTrigger->AttachToComponent(
				JeoulSkeletalMesh, 
				FAttachmentTransformRules::SnapToTargetNotIncludingScale, 
				TEXT("joint5") // 왼쪽 저울판 위치에 정확히 마운트
			);
		}
	}

	if (ExternalSwitchActor)
	{
		ExternalSwitch = ExternalSwitchActor->FindComponentByClass<UInteractionSwitchComponent>();
	}

	if (HasAuthority() && ExternalSwitch)
	{
		ExternalSwitch->OnSwitchActivatedChanged.AddUObject(this, &AJeoul::OnCheckButtonPressed);
	}
}

void AJeoul::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AJeoul, CurrentState);
	DOREPLIFETIME(AJeoul, CurrentTiltState);
}

bool AJeoul::HasBothPlayersOnPlate() const
{
	if (!OverlapTrigger) return false;

	TArray<AActor*> OverlappingActors;
	OverlapTrigger->GetOverlappingActors(OverlappingActors);

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

void AJeoul::SetupBlockingMesh(UPrimitiveComponent* Mesh, ECollisionResponse VisibilityResponse)
{
	if (!Mesh) return;
	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, VisibilityResponse);
}

void AJeoul::OnCheckButtonPressed(bool bActivated)
{
	if (!HasAuthority() || !bActivated || !ExternalSwitch) return;

	if (CurrentState == EJeoulState::Idle)
	{
		Server_CheckBalance();
	}
	else if (CurrentState == EJeoulState::Resolved || CurrentState == EJeoulState::Checking)
	{
		if (APS3PlayerCharacter* InteractingChar = Cast<APS3PlayerCharacter>(
			ExternalSwitch->GetInteractingActor()))
		{
			if (APS3PlayerController* PC = Cast<APS3PlayerController>(InteractingChar->GetController()))
			{
				RequestCutsceneReturn(PC);
			}
		}
	}
}

void AJeoul::Multicast_AlignPlayerCharacter_Implementation(APS3PlayerCharacter* TargetCharacter, FVector TargetLocation,
														   FRotator TargetRotation, USceneComponent* AttachTarget)
{
	if (!IsValid(TargetCharacter)) return;

	TargetCharacter->SetCanUseFieldControls(false);

	// 1. 진짜 캐릭터 숨기기 및 이동 비활성화
	if (UCharacterMovementComponent* MovementComp = TargetCharacter->GetCharacterMovement())
	{
		MovementComp->StopMovementImmediately();
		MovementComp->DisableMovement();
	}
	if (UCapsuleComponent* Capsule = TargetCharacter->GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	TargetCharacter->SetActorHiddenInGame(true);

	// 2. 더미 메쉬 선택
	USkeletalMeshComponent* TargetDummy = (AttachTarget == Player1Spot) ? DummyPlayer1Mesh : DummyPlayer2Mesh;

	if (TargetDummy && TargetCharacter->GetMesh())
	{
		USkeletalMeshComponent* RealMesh = TargetCharacter->GetMesh();

		// ★ [핵심] 부모(저울 뼈대)의 비정상 스케일을 무시하고 독립적인 절대 스케일 유지
		TargetDummy->SetAbsolute(false, false, true); 
	
		// ★ [핵심] 진짜 캐릭터의 정상 스케일, 상대 위치/회전 오프셋 복사
		TargetDummy->SetWorldScale3D(RealMesh->GetComponentScale());
		TargetDummy->SetRelativeLocation(RealMesh->GetRelativeLocation());
		TargetDummy->SetRelativeRotation(RealMesh->GetRelativeRotation());

		// 메쉬 및 머티리얼 복사
		TargetDummy->SetSkeletalMesh(RealMesh->GetSkeletalMeshAsset());
		for (int32 i = 0; i < RealMesh->GetNumMaterials(); ++i)
		{
			TargetDummy->SetMaterial(i, RealMesh->GetMaterial(i));
		}

		// 애니메이션 블루프린트 복사
		if (UClass* AnimClass = RealMesh->GetAnimClass())
		{
			TargetDummy->SetAnimInstanceClass(AnimClass);
		}

		TargetDummy->SetVisibility(true);
	}

	if (AController* Controller = TargetCharacter->GetController())
	{
		Controller->SetControlRotation(TargetRotation);
	}
}

void AJeoul::Multicast_RestorePlayerCharacter_Implementation(APS3PlayerCharacter* TargetCharacter)
{
	if (!IsValid(TargetCharacter)) return;

	// 1. 더미 메쉬 모두 숨기기
	if (DummyPlayer1Mesh) DummyPlayer1Mesh->SetVisibility(false);
	if (DummyPlayer2Mesh) DummyPlayer2Mesh->SetVisibility(false);

	// 2. 현재 저울판(PlayerSpot)의 최종 위치로 진짜 캐릭터 위치 텔레포트
	USceneComponent* TargetSpot = Player1Spot;
	if (Player1Spot && Player2Spot)
	{
		float Dist1 = FVector::DistSquared(TargetCharacter->GetActorLocation(), Player1Spot->GetComponentLocation());
		float Dist2 = FVector::DistSquared(TargetCharacter->GetActorLocation(), Player2Spot->GetComponentLocation());
		if (Dist2 < Dist1)
		{
			TargetSpot = Player2Spot;
		}
	}

	if (TargetSpot)
	{
		FVector FinalLocation = TargetSpot->GetComponentLocation();
		FRotator FinalRotation = TargetSpot->GetComponentRotation();

		if (UCapsuleComponent* Capsule = TargetCharacter->GetCapsuleComponent())
		{
			FinalLocation.Z += Capsule->GetScaledCapsuleHalfHeight();
		}

		TargetCharacter->SetActorLocationAndRotation(FinalLocation, FRotator(0.0f, FinalRotation.Yaw, 0.0f), false, nullptr, ETeleportType::TeleportPhysics);

		if (AController* Controller = TargetCharacter->GetController())
		{
			Controller->SetControlRotation(FRotator(0.0f, FinalRotation.Yaw, 0.0f));
		}
	}

	// 3. 진짜 캐릭터 외형 및 콜리전/무브먼트 원복
	TargetCharacter->SetActorHiddenInGame(false);

	if (UCapsuleComponent* Capsule = TargetCharacter->GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	}

	if (UCharacterMovementComponent* MovementComp = TargetCharacter->GetCharacterMovement())
	{
		MovementComp->SetMovementMode(MOVE_Walking);
	}

	TargetCharacter->SetCanUseFieldControls(true);
}

void AJeoul::AlignPlayersAndDumbbells()
{
	if (!HasAuthority() || !OverlapTrigger) return;

	TArray<AActor*> OverlappingActors;
	OverlapTrigger->GetOverlappingActors(OverlappingActors);

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

				// ★ PlateTrigger 대신 TargetSpot을 넘겨주어야 DummyPlayer1 / DummyPlayer2가 정상 구분됨
				Multicast_AlignPlayerCharacter(Cast<APS3PlayerCharacter>(Character), TargetLocation, TargetRotation,
											   TargetSpot);
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
						PlateTrigger,
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
	if (!HasAuthority() || !IsValid(RequestingController)) return;

	const TArray<TWeakObjectPtr<APS3PlayerController>> Participants = MoveTemp(CutsceneParticipants);
	CutsceneParticipants.Reset();

	for (const TWeakObjectPtr<APS3PlayerController>& Participant : Participants)
	{
		if (APS3PlayerController* PlayerController = Participant.Get())
		{
			if (APS3PlayerCharacter* TargetChar = Cast<APS3PlayerCharacter>(PlayerController->GetPawn()))
			{
				Multicast_RestorePlayerCharacter(TargetChar);
			}

			PlayerController->Client_EndJeoulCutscene(this);
		}
	}
}

float AJeoul::CalculateWeightOnPlate(UBoxComponent* InOverlapTrigger)
{
	if (!InOverlapTrigger) return 0.0f;

	TArray<AActor*> OverlappingActors;
	InOverlapTrigger->GetOverlappingActors(OverlappingActors);

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
						PlateTrigger,
						FAttachmentTransformRules::KeepWorldTransform
					);
				}
			}
		}
		else if (APS3PlayerCharacter* PlayerChar = Cast<APS3PlayerCharacter>(Actor))
		{
			if (ADumbbell* HeldDumbbell = PlayerChar->GetHeldDumbbell())
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
	OnJeoulCheckStarted.Broadcast();
}

void AJeoul::Multicast_OnJeoulCheckFinished_Implementation(bool bIsSuccess)
{
	OnJeoulCheckFinished.Broadcast(bIsSuccess);
}

void AJeoul::Multicast_PlayTiltAnimation_Implementation(EJeoulTiltState TiltState)
{
	if (!JeoulSkeletalMesh) return;

	UAnimInstance* AnimInstance = JeoulSkeletalMesh->GetAnimInstance();
	if (!AnimInstance) return;

	if (TiltState == EJeoulTiltState::TiltLeft && LeftTiltMontage)
	{
		AnimInstance->Montage_Play(LeftTiltMontage);
	}
	else if (TiltState == EJeoulTiltState::TiltRight && RightTiltMontage)
	{
		AnimInstance->Montage_Play(RightTiltMontage);
	}
	else if (TiltState == EJeoulTiltState::Balanced)
	{
		AnimInstance->Montage_Stop(0.2f);
	}
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

		if (ExternalSwitch)
		{
			ExternalSwitch->ResetSwitch();
		}
		return;
	}

	CurrentState = EJeoulState::Checking;

	// ★ 1. 콜리전이 켜져 있는 상태에서 플레이어를 구하고 카메라 전환(Client_BeginJeoulCutscene) RPC를 먼저 실행
	TArray<AActor*> PlayersOnPlate;
	OverlapTrigger->GetOverlappingActors(PlayersOnPlate, APS3PlayerCharacter::StaticClass());

	for (AActor* PlayerActor : PlayersOnPlate)
	{
		APS3PlayerCharacter* PlayerCharacter = Cast<APS3PlayerCharacter>(PlayerActor);
		if (!IsValid(PlayerCharacter)) continue;

		if (APS3PlayerController* PlayerController = Cast<APS3PlayerController>(PlayerCharacter->GetController()))
		{
			CutsceneParticipants.AddUnique(TWeakObjectPtr<APS3PlayerController>(PlayerController));
			PlayerController->Client_BeginJeoulCutscene(this);
		}
	}

	// ★ 2. 카메라 전환 요청 직후 플레이어 비활성화 및 더미 메쉬 교체 진행
	AlignPlayersAndDumbbells();

	Multicast_OnJeoulCheckStarted();

	const float TotalWeight = CalculateWeightOnPlate(OverlapTrigger);

	float JudgeWeight = 0.0f;
	if (APS3GameStateS4* GS = GetWorld()->GetGameState<APS3GameStateS4>())
	{
		JudgeWeight = GS->GetTargetBalancedWeight();
	}

	float WeightDifference = JudgeWeight - TotalWeight;

	if (FMath::IsNearlyEqual(TotalWeight, JudgeWeight, 0.01f) && TotalWeight > 0.0f)
	{
		CurrentTiltState = EJeoulTiltState::Balanced;
	}
	else if (WeightDifference > 0.0f)
	{
		CurrentTiltState = EJeoulTiltState::TiltLeft;
	}
	else
	{
		CurrentTiltState = EJeoulTiltState::TiltRight;
	}

	Multicast_PlayTiltAnimation(CurrentTiltState);

	UE_LOG(LogTemp, Warning, TEXT("[Jeoul] 무게 차이: %f, 현재 무게: %f, 목표 무게: %f"), WeightDifference, TotalWeight, JudgeWeight);

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

			if (ExternalSwitch)
			{
				ExternalSwitch->ResetSwitch();
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[Jeoul] 무게 불균형! 저울대 기울기 원위치 리셋"));

			if (ExternalSwitch)
			{
				ExternalSwitch->ResetSwitch();
			}

			FTimerHandle ResetTimer;
			GetWorldTimerManager().SetTimer(ResetTimer, [this]()
			{
				CurrentTiltState = EJeoulTiltState::Balanced;
				CurrentState = EJeoulState::Idle;
				
				Multicast_PlayTiltAnimation(EJeoulTiltState::Balanced);
				Multicast_OnJeoulCheckFinished(false);
			}, ResetBeamTime, false);
		}
	}, CutSceneTime, false);
}