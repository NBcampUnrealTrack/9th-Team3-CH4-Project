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

void AJeoul::Multicast_RestorePlayerCharacter_Implementation(APS3PlayerCharacter* TargetCharacter)
{
	if (!IsValid(TargetCharacter)) return;

	TargetCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	FRotator CurrentRot = TargetCharacter->GetActorRotation();
	FRotator UprightRot = FRotator(0.0f, CurrentRot.Yaw, 0.0f);
	TargetCharacter->SetActorRotation(UprightRot);

	if (AController* Controller = TargetCharacter->GetController())
	{
		Controller->SetControlRotation(UprightRot);
	}

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

	FRotator UprightTargetRotation = FRotator(0.0f, TargetRotation.Yaw, 0.0f);

	TargetCharacter->SetActorLocationAndRotation(TargetLocation, UprightTargetRotation, false, nullptr,
	                                             ETeleportType::TeleportPhysics);

	if (AttachTarget)
	{
		TargetCharacter->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepWorldTransform);
	}

	if (AController* Controller = TargetCharacter->GetController())
	{
		Controller->SetControlRotation(UprightTargetRotation);
	}
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

				Multicast_AlignPlayerCharacter(Cast<APS3PlayerCharacter>(Character), TargetLocation, TargetRotation,
				                               PlateTrigger);
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

	AlignPlayersAndDumbbells();

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