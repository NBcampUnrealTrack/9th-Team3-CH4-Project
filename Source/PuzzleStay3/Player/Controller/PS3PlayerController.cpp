#include "PS3PlayerController.h"
#include "Component/FakeDeathTrapComponent.h"
#include "EngineUtils.h"
#include "Object/Jeoul.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Player/Character/PS3PlayerCharacter.h"
#include "Component/CustomVoiceComponent.h"
#include "Component/VoicePluginControlComponent.h"
#include "Player/PlayerState/PS3PlayerState.h"
#include "Player/Online/PS3LobbySubsystem.h"
#include "TimerManager.h"
#include "Core/GameMode/PS3GameModeS3.h"
#include "Data/Delegates/UIDelegatesSubsystem.h"
#include "UI/HUD/PlayerHUD.h"
#include "UI/ViewModel/PS3ViewModel.h"


class APS3GameModeS3;

APS3PlayerController::APS3PlayerController()
{
	bShowMouseCursor = false;
	
	VoicePluginControlComponent =
	CreateDefaultSubobject<UVoicePluginControlComponent>(
	TEXT("VoicePluginControlComponent"));
	
	VoiceComponent = CreateDefaultSubobject<UCustomVoiceComponent>(
	TEXT("VoiceComponent"));
	
}

void APS3PlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		if (APS3GameModeS3* GameMode = GetWorld()->GetAuthGameMode<APS3GameModeS3>())
		{
			// 먼저 구독해야 등록 직후 Broadcast가 발생해도 받을 수 있습니다.
			Stage3VisibilityDelegateHandle =
				GameMode->OnRandomVisibleResultsChanged.AddUObject(
					this, &ThisClass::HandleRandomVisibleResultsChanged);

			GameMode->RegisterPlayerController(this);
		}
	}
	
	ConfigureLocalInput();
	RefreshVoiceStateBinding();
	RefreshLifeStateBinding();

	if (IsLocalController())
	{
		StartVoiceRestore();

		GetWorldTimerManager().SetTimer(
			LifeUIInitializationTimerHandle,
			this,
			&ThisClass::RefreshLifeStateBinding,
			0.5f,
			false);
		
		PS3_BROADCAST_TO_MVVM_OneParams(OnFieldPlayer_UI, true);
	}
	
	
}

void APS3PlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	EndJeoulCutscene();
	GetWorldTimerManager().ClearTimer(Stage3VisibilityTimerHandle);
	GetWorldTimerManager().ClearTimer(LifeUIInitializationTimerHandle);
	GetWorldTimerManager().ClearTimer(VoiceRestoreTimerHandle);

	if (IsValid(BoundLifePlayerState))
	{
		BoundLifePlayerState->OnLifeCountChanged.RemoveDynamic(
			this,
			&ThisClass::HandleLifeCountChanged);
	}
	
	if (HasAuthority())
	{
		if (APS3GameModeS3* GameMode = GetWorld()->GetAuthGameMode<APS3GameModeS3>())
		{
			if (Stage3VisibilityDelegateHandle.IsValid())
			{
				GameMode->OnRandomVisibleResultsChanged.Remove(
					Stage3VisibilityDelegateHandle);
			}
			GameMode->UnregisterPlayerController(this);
		}
	}

	BoundLifePlayerState = nullptr;

	Super::EndPlay(EndPlayReason);
}

void APS3PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (APS3PlayerState* PS3PlayerState = GetPlayerState<APS3PlayerState>())
	{
		if (PS3PlayerState->IsRespawning())
		{
			PS3PlayerState->FinishRespawn();
		}
	}

	Client_RestoreAfterRespawn();
}

void APS3PlayerController::Client_PrepareForRespawn_Implementation()
{
	if (!IsLocalController())
	{
		return;
	}

	EndJeoulCutscene();
	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);

	if (IsValid(VoiceComponent))
	{
		VoiceComponent->StopPushToTalk();
	}
}

void APS3PlayerController::Client_RestoreAfterRespawn_Implementation()
{
	if (!IsLocalController())
	{
		return;
	}

	EndJeoulCutscene();
	if (IsValid(GetPawn())) SetViewTarget(GetPawn());
	ResetIgnoreMoveInput();
	ResetIgnoreLookInput();
	ConfigureLocalInput();
	RefreshVoiceStateBinding();
	RefreshLifeStateBinding();
}

void APS3PlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	ConfigureLocalInput();
	RefreshVoiceStateBinding();
	RefreshLifeStateBinding();
	StartVoiceRestore();
}

void APS3PlayerController::StartVoiceRestore()
{
	if (!IsLocalController() || !IsValid(GetWorld())) return;

	VoiceRestoreAttemptCount = 0;
	GetWorldTimerManager().ClearTimer(VoiceRestoreTimerHandle);
	TryRestoreLobbyVoice();

	if (!IsValid(VoicePluginControlComponent) || !VoicePluginControlComponent->IsVoiceReady())
	{
		GetWorldTimerManager().SetTimer(
			VoiceRestoreTimerHandle,
			this,
			&ThisClass::TryRestoreLobbyVoice,
			VoiceRestoreRetryInterval,
			true);
	}
}

void APS3PlayerController::TryRestoreLobbyVoice()
{
	if (!IsLocalController() || !IsValid(VoicePluginControlComponent))
	{
		GetWorldTimerManager().ClearTimer(VoiceRestoreTimerHandle);
		return;
	}

	++VoiceRestoreAttemptCount;
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UPS3LobbySubsystem* LobbySubsystem = GameInstance->GetSubsystem<UPS3LobbySubsystem>())
		{
			LobbySubsystem->InitializeLocalVoice();
		}
	}

	if (VoicePluginControlComponent->IsVoiceReady())
	{
		GetWorldTimerManager().ClearTimer(VoiceRestoreTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("EOS lobby voice restored after map travel."));
		return;
	}

	if (VoiceRestoreAttemptCount >= MaxVoiceRestoreAttempts)
	{
		GetWorldTimerManager().ClearTimer(VoiceRestoreTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("EOS lobby voice restore timed out after %d attempts."), VoiceRestoreAttemptCount);
	}
}

void APS3PlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	RefreshVoiceStateBinding();
	RefreshLifeStateBinding();
}

void APS3PlayerController::RefreshVoiceStateBinding()
{
	if (!IsValid(VoiceComponent))
	{
		return;
	}

	VoiceComponent->BindPlayerState(
		GetPlayerState<APS3PlayerState>()
	);
}

void APS3PlayerController::RefreshLifeStateBinding()
{
	if (!IsLocalController())
	{
		return;
	}

	APS3PlayerState* NewPlayerState =
		GetPlayerState<APS3PlayerState>();

	if (BoundLifePlayerState != NewPlayerState)
	{
		if (IsValid(BoundLifePlayerState))
		{
			BoundLifePlayerState->OnLifeCountChanged.RemoveDynamic(
				this,
				&ThisClass::HandleLifeCountChanged);
		}

		BoundLifePlayerState = NewPlayerState;

		if (IsValid(BoundLifePlayerState))
		{
			BoundLifePlayerState->OnLifeCountChanged.AddUniqueDynamic(
				this,
				&ThisClass::HandleLifeCountChanged);
		}
	}

	if (IsValid(BoundLifePlayerState))
	{
		HandleLifeCountChanged(
			BoundLifePlayerState->GetCurrentLifeCount());
	}
}

UPS3ViewModel* APS3PlayerController::GetPS3ViewModel() const
{
	const APlayerHUD* PlayerHUD = Cast<APlayerHUD>(GetHUD());
	return IsValid(PlayerHUD) ? PlayerHUD->GetViewModel() : nullptr;
}

void APS3PlayerController::HandleLifeCountChanged(const int32 NewLifeCount)
{
	if (!IsLocalController())
	{
		return;
	}

	if (UPS3ViewModel* ViewModel = GetPS3ViewModel())
	{
		ViewModel->RequestUpdateLifeCount(
			NewLifeCount,
			MaxLifeCountForUI);
	}
}

void APS3PlayerController::ConfigureLocalInput()
{
	if (bLocalInputConfigured || !IsLocalController())
	{
		return;
	}

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;

	if (IsValid(InputMappingContext))
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			InputSubsystem->AddMappingContext(InputMappingContext, 0);
			bLocalInputConfigured = true;
		}
	}
}

void APS3PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (!IsValid(EIC))
	{
		return;
	}

	if (IsValid(MoveAction))
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::HandleMoveInput);
	}

	if (IsValid(LookAction))
	{
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::HandleLookInput);
	}

	if (IsValid(JumpAction))
	{
		EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::HandleJumpStarted);
		EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ThisClass::HandleJumpCompleted);
	}

	if (IsValid(InteractAction))
	{
		EIC->BindAction(InteractAction, ETriggerEvent::Started, this, &ThisClass::HandleInteractStarted);
	}

	if (IsValid(DropAction))
	{
		EIC->BindAction(DropAction, ETriggerEvent::Started, this, &ThisClass::HandleDropStarted);
	}
	
	if (IsValid(PushToTalkAction))
	{
		EIC->BindAction(PushToTalkAction, ETriggerEvent::Started,this, &ThisClass::HandleVoiceStarted);
		EIC->BindAction(PushToTalkAction, ETriggerEvent::Completed,this, &ThisClass::HandleVoiceStopped);
		EIC->BindAction(PushToTalkAction, ETriggerEvent::Canceled,this, &ThisClass::HandleVoiceStopped);
	}
}

void APS3PlayerController::HandleMoveInput(const FInputActionValue& InValue)
{
	if (IsJeoulCutsceneActive()) return;
	if (APS3PlayerCharacter* PlayerCharacter = GetPawn<APS3PlayerCharacter>())
	{
		PlayerCharacter->Move(InValue.Get<FVector2D>());
	}
}

void APS3PlayerController::HandleLookInput(const FInputActionValue& InValue)
{
	if (IsJeoulCutsceneActive()) return;
	if (APS3PlayerCharacter* PlayerCharacter = GetPawn<APS3PlayerCharacter>())
	{
		PlayerCharacter->Look(InValue.Get<FVector2D>());
	}
}

void APS3PlayerController::HandleJumpStarted()
{
	if (IsJeoulCutsceneActive()) return;
	if (APS3PlayerCharacter* PlayerCharacter = GetPawn<APS3PlayerCharacter>())
	{
		PlayerCharacter->StartJump();
	}
}

void APS3PlayerController::HandleJumpCompleted()
{
	if (APS3PlayerCharacter* PlayerCharacter = GetPawn<APS3PlayerCharacter>())
	{
		PlayerCharacter->StopJump();
	}
}

void APS3PlayerController::HandleInteractStarted()
{
	if (IsJeoulCutsceneActive())
	{
		if (JeoulCutsceneState == EJeoulCutsceneState::AwaitingReturn)
		{
			Server_RequestJeoulCutsceneReturn(ActiveCutsceneJeoul.Get());
		}
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("[Input] Interact pressed"));

	if (APS3PlayerCharacter* PlayerCharacter = GetPawn<APS3PlayerCharacter>())
	{
		PlayerCharacter->TryInteract();
	}
}

void APS3PlayerController::HandleDropStarted()
{
	if (IsJeoulCutsceneActive()) return;
	UE_LOG(LogTemp, Warning, TEXT("[Input] Drop pressed"));

	if (APS3PlayerCharacter* PlayerCharacter = GetPawn<APS3PlayerCharacter>())
	{
		PlayerCharacter->TryDropHeldObject();
	}
}


void APS3PlayerController::HandleVoiceStarted()
{
	UE_LOG(LogTemp, Warning, TEXT("Voice: V pressed"));
	if (IsValid(VoiceComponent))
	{
		VoiceComponent->StartPushToTalk();
	}
}

void APS3PlayerController::HandleVoiceStopped()
{
	UE_LOG(LogTemp, Warning, TEXT("Voice: V released"));
	if (IsValid(VoiceComponent))
	{
		VoiceComponent->StopPushToTalk();
	}
}

void APS3PlayerController::Client_ReceiveStage3Visibility_Implementation(const TArray<int32>& TrapIds, const TArray<bool>& Results)
{
	if (!IsLocalController() || TrapIds.Num() != Results.Num()) return;
	Stage3VisibilityByTrapId.Empty();
	for (int32 Index = 0; Index < TrapIds.Num(); ++Index)
	{
		Stage3VisibilityByTrapId.Add(TrapIds[Index], Results[Index]);
	}
	ApplyStage3Visibility();
	// 늦은 BeginPlay 및 스트리밍으로 다시 나타나는 함정에도 저장된 결과 적용.
	GetWorldTimerManager().SetTimer(Stage3VisibilityTimerHandle, this, &ThisClass::ApplyStage3Visibility, 0.5f, true);
}

void APS3PlayerController::ApplyStage3Visibility()
{
	if (!IsLocalController()) return;
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		TInlineComponentArray<UFakeDeathTrapComponent*> Traps;
		It->GetComponents(Traps);
		for (UFakeDeathTrapComponent* Trap : Traps)
		{
			if (!IsValid(Trap) || !Trap->HasBegunPlay()) continue;
			if (const bool* bVisible = Stage3VisibilityByTrapId.Find(Trap->GetTrapId()))
			{
				Trap->ApplyLocalFakeTrapState(*bVisible);
			}
		}
	}
}



void APS3PlayerController::Client_BeginJeoulCutscene_Implementation(AJeoul* Jeoul)
{
	if (!IsLocalController() || !IsValid(Jeoul)) return;
	UCameraComponent* CutsceneCamera = Jeoul->GetCutsceneCamera();
	if (!IsValid(CutsceneCamera) || !CutsceneCamera->IsActive()) return;
	if (IsJeoulCutsceneActive() && ActiveCutsceneJeoul.Get() == Jeoul) return;
	EndJeoulCutscene();

	PreviousCutsceneViewTarget = GetViewTarget();
	ActiveCutsceneJeoul = Jeoul;
	JeoulCutsceneState = EJeoulCutsceneState::Playing;
	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);
	if (APS3PlayerCharacter* PlayerCharacter = GetPawn<APS3PlayerCharacter>())
	{
		PlayerCharacter->StopJump();
		PlayerCharacter->GetCharacterMovement()->StopMovementImmediately();
		PlayerCharacter->ConsumeMovementInputVector();
		CutsceneCharacter = PlayerCharacter;
		CutsceneStartRotation = PlayerCharacter->GetActorRotation();
		CutsceneTurnElapsed = 0.0f;
		UCharacterMovementComponent* Movement = PlayerCharacter->GetCharacterMovement();
		bSavedOrientRotationToMovement = Movement->bOrientRotationToMovement;
		bSavedUseControllerDesiredRotation = Movement->bUseControllerDesiredRotation;
		bSavedUseControllerRotationYaw = PlayerCharacter->bUseControllerRotationYaw;
		Movement->bOrientRotationToMovement = false;
		Movement->bUseControllerDesiredRotation = false;
		PlayerCharacter->bUseControllerRotationYaw = false;
	}
	JeoulCheckFinishedHandle = Jeoul->OnJeoulCheckFinished.AddUObject(this, &ThisClass::HandleJeoulCheckFinished);
	Jeoul->OnDestroyed.AddUniqueDynamic(this, &ThisClass::HandleCutsceneTargetDestroyed);
	SetViewTargetWithBlend(Jeoul, JeoulCameraBlendTime);
}

void APS3PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	if (!IsLocalController() || !IsJeoulCutsceneActive()) return;

	APS3PlayerCharacter* CutscenePlayerCharacter = CutsceneCharacter.Get();
	AJeoul* Jeoul = ActiveCutsceneJeoul.Get();
	if (!IsValid(CutscenePlayerCharacter) || CutscenePlayerCharacter != GetPawn() || !IsValid(Jeoul)
		|| !IsValid(Jeoul->GetCutsceneCamera()))
	{
		EndJeoulCutscene();
		return;
	}

	// 로컬 연출 전용. 다른 클라이언트에 보이는 회전은 서버 연동이 필요합니다.
	FVector Direction = Jeoul->GetCutsceneCamera()->GetComponentLocation() - CutscenePlayerCharacter->GetActorLocation();
	Direction.Z = 0.0f;
	if (Direction.IsNearlyZero()) return;

	CutsceneTurnElapsed += DeltaTime;
	const float Alpha = JeoulCharacterTurnTime > 0.0f
		? FMath::Clamp(CutsceneTurnElapsed / JeoulCharacterTurnTime, 0.0f, 1.0f) : 1.0f;
	const float Yaw = CutsceneStartRotation.Yaw
		+ FMath::FindDeltaAngleDegrees(CutsceneStartRotation.Yaw, Direction.Rotation().Yaw) * Alpha;
	CutscenePlayerCharacter->SetActorRotation(FRotator(0.0f, Yaw, 0.0f));
}

void APS3PlayerController::Server_RequestJeoulCutsceneReturn_Implementation(AJeoul* Jeoul)
{
	if (IsValid(Jeoul)) Jeoul->RequestCutsceneReturn(this);
}

void APS3PlayerController::Client_EndJeoulCutscene_Implementation(AJeoul* Jeoul)
{
	if (IsLocalController() && IsValid(Jeoul) && ActiveCutsceneJeoul.Get() == Jeoul)
	{
		EndJeoulCutscene();
	}
}

void APS3PlayerController::EndJeoulCutscene()
{
	if (!IsJeoulCutsceneActive()) return;
	if (AJeoul* Jeoul = ActiveCutsceneJeoul.Get())
	{
		Jeoul->OnJeoulCheckFinished.Remove(JeoulCheckFinishedHandle);
		Jeoul->OnDestroyed.RemoveDynamic(this, &ThisClass::HandleCutsceneTargetDestroyed);
	}
	JeoulCheckFinishedHandle.Reset();
	if (APS3PlayerCharacter* CutscenePlayerCharacter = CutsceneCharacter.Get())
	{
		UCharacterMovementComponent* Movement = CutscenePlayerCharacter->GetCharacterMovement();
		Movement->bOrientRotationToMovement = bSavedOrientRotationToMovement;
		Movement->bUseControllerDesiredRotation = bSavedUseControllerDesiredRotation;
		CutscenePlayerCharacter->bUseControllerRotationYaw = bSavedUseControllerRotationYaw;
	}
	CutsceneCharacter.Reset();
	ActiveCutsceneJeoul.Reset();
	JeoulCutsceneState = EJeoulCutsceneState::Inactive;
	// 이 컷신에서 추가한 입력 잠금 한 번만 해제합니다.
	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);
	AActor* RestoreTarget = PreviousCutsceneViewTarget.Get();
	if (!IsValid(RestoreTarget)) RestoreTarget = GetPawn();
	if (IsValid(RestoreTarget)) SetViewTargetWithBlend(RestoreTarget, JeoulCameraBlendTime);
	PreviousCutsceneViewTarget.Reset();
}

void APS3PlayerController::Client_ShowTextNotify_Implementation(EPS3TextNotifyType NotifyType)
{
	if (!IsLocalController()) return;

	PS3_BROADCAST_TO_MVVM_OneParams(OnTextNotify_UI, NotifyType);
}

void APS3PlayerController::HandleJeoulCheckFinished(bool bIsSuccess)
{
	if (JeoulCutsceneState == EJeoulCutsceneState::Playing)
	{
		JeoulCutsceneState = EJeoulCutsceneState::AwaitingReturn;
	}
}

void APS3PlayerController::HandleCutsceneTargetDestroyed(AActor* DestroyedActor)
{
	EndJeoulCutscene();
}

bool APS3PlayerController::InitializeVoiceSystem(const int32 LocalUserNum)
{
	if (!IsLocalController() || !IsValid(VoicePluginControlComponent))
	{
		return false;
	}

	return VoicePluginControlComponent->InitializeEOSVoice(LocalUserNum);
}

void APS3PlayerController::ShutdownVoiceSystem()
{
	if (IsValid(VoicePluginControlComponent))
	{
		VoicePluginControlComponent->ShutdownEOSVoice();
	}
}

void APS3PlayerController::HandleRandomVisibleResultsChanged(
	APS3PlayerController* TargetController,
	const TArray<int32>& TrapIds,
	const TArray<bool>& Results)
{
	if (!HasAuthority() || TargetController != this)
	{
		return;
	}

	if (TrapIds.Num() != Results.Num())
	{
		return;
	}

	// 이 Controller를 소유한 클라이언트로 전달합니다.
	Client_ReceiveStage3Visibility(TrapIds, Results);
}
