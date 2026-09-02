#include "PS3PlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Player/Character/PS3PlayerCharacter.h"
#include "Component/CustomVoiceComponent.h"


APS3PlayerController::APS3PlayerController()
{
	bShowMouseCursor = false;
	
	VoiceComponent = CreateDefaultSubobject<UCustomVoiceComponent>(
	TEXT("VoiceComponent"));
	
}

void APS3PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
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
	if (APS3PlayerCharacter* PlayerCharacter = GetPawn<APS3PlayerCharacter>())
	{
		PlayerCharacter->Move(InValue.Get<FVector2D>());
	}
}

void APS3PlayerController::HandleLookInput(const FInputActionValue& InValue)
{
	if (APS3PlayerCharacter* PlayerCharacter = GetPawn<APS3PlayerCharacter>())
	{
		PlayerCharacter->Look(InValue.Get<FVector2D>());
	}
}

void APS3PlayerController::HandleJumpStarted()
{
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
	if (APS3PlayerCharacter* PlayerCharacter = GetPawn<APS3PlayerCharacter>())
	{
		PlayerCharacter->TryInteract();
	}
}

void APS3PlayerController::HandleDropStarted()
{
	if (APS3PlayerCharacter* PlayerCharacter = GetPawn<APS3PlayerCharacter>())
	{
		PlayerCharacter->TryDropHeldObject();
	}
}


void APS3PlayerController::HandleVoiceStarted()
{
	UE_LOG(LogTemp, Warning, TEXT("Voice: V pressed"));
	VoiceComponent->StartPushToTalk();
}

void APS3PlayerController::HandleVoiceStopped()
{
	UE_LOG(LogTemp, Warning, TEXT("Voice: V released"));
	VoiceComponent->StopPushToTalk();
}