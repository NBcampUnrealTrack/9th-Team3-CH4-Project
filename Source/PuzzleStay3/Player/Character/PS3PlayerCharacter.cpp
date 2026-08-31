
#include "PS3PlayerCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/PlayerState/PS3PlayerState.h"
#include "Component/InteractionSwitchComponent.h"



APS3PlayerCharacter::APS3PlayerCharacter()
{
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 400.f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = false;
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
}

void APS3PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
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
		EIC->BindAction(InteractAction,ETriggerEvent::Started,this,&ThisClass::HandleInteractStarted);
	}
	
	if (IsValid(DropAction))
	{
		EIC->BindAction(DropAction, ETriggerEvent::Started,this,&ThisClass::HandleDropStarted);
	}
}


void APS3PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled() && IsValid(InputMappingContext))
	{
		if (const APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
			{
				InputSubsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}

bool APS3PlayerCharacter::CanUseFieldControls() const
{
	const APS3PlayerState* PS3PlayerState = GetPlayerState<APS3PlayerState>();
	return !IsValid(PS3PlayerState) || PS3PlayerState->GetPlayerRole() != EPS3PlayerRole::Screen;
}


void APS3PlayerCharacter::HandleMoveInput(const FInputActionValue& InValue)
{
	if (!IsValid(Controller) || !CanUseFieldControls()) return;

	const FVector2D InMovementVector = InValue.Get<FVector2D>();

	const FRotator ControlRotation = Controller->GetControlRotation();
	const FRotator ControlYawRotation(0.0f, ControlRotation.Yaw, 0.0f);

	const FVector ForwardDirection = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, InMovementVector.X);
	AddMovementInput(RightDirection, InMovementVector.Y);
}

void APS3PlayerCharacter::HandleLookInput(const FInputActionValue& InValue)
{
	if (!IsValid(Controller) || !CanUseFieldControls()) return;


	if (IsLocallyControlled() == true)
	{
		const FVector2D InLookVector = InValue.Get<FVector2D>();

		AddControllerYawInput(InLookVector.X);
		AddControllerPitchInput(InLookVector.Y);
	}
}

void APS3PlayerCharacter::HandleJumpStarted()
{
	if (CanUseFieldControls())
	{
		Jump();
	}
}

void APS3PlayerCharacter::HandleJumpCompleted()
{
	StopJumping();
}

void APS3PlayerCharacter::HandleDropStarted()
{
	if (!IsLocallyControlled() || !CanUseFieldControls())
	{
		return;
	}

	Server_TryDropHeldObject();
}


void APS3PlayerCharacter::Server_TryDropHeldObject_Implementation()
{
	if (!CanUseFieldControls())
	{
		return;
	}

	// 실제 드롭 컴포넌트가 생기면:
	// GrabComponent->TryDropHeldObject();
}

//상호작용했다는 것을 서버에 알림
void APS3PlayerCharacter::HandleInteractStarted()
{
	if (!CanUseFieldControls())
	{
		return;
	}

	Server_TryInteract();
}

void APS3PlayerCharacter::Server_TryInteract_Implementation()
{
	if (!IsValid(Controller) || !CanUseFieldControls())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	FVector ViewLocation;
	FRotator ViewRotation;
	Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);

	const FVector TraceEnd =
		ViewLocation + ViewRotation.Vector() * InteractionDistance;

	FCollisionQueryParams QueryParams(
		SCENE_QUERY_STAT(PlayerInteractionTrace),
		false,
		this
	);

	FHitResult HitResult;
	const bool bHit = World->LineTraceSingleByChannel(
		HitResult,
		ViewLocation,
		TraceEnd,
		ECC_Visibility,
		QueryParams
	);

	if (!bHit || !IsValid(HitResult.GetActor()))
	{
		return;
	}

	UInteractionSwitchComponent* InteractionComponent =
		HitResult.GetActor()->FindComponentByClass<UInteractionSwitchComponent>();

	if (!IsValid(InteractionComponent))
	{
		return;
	}

	InteractionComponent->TryInteract(this);
}