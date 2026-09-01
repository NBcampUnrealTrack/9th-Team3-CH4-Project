
#include "PS3PlayerCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
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

bool APS3PlayerCharacter::CanUseFieldControls() const
{
	const APS3PlayerState* PS3PlayerState = GetPlayerState<APS3PlayerState>();
	if (!IsValid(PS3PlayerState)|| PS3PlayerState->GetPlayerRole() != EPS3PlayerRole::Field)
	{
		return false;
	}

	return true;
}


void APS3PlayerCharacter::Move(const FVector2D& InMovementVector)
{
	if (!IsValid(Controller) || !CanUseFieldControls()) return;

	const FRotator ControlRotation = Controller->GetControlRotation();
	const FRotator ControlYawRotation(0.0f, ControlRotation.Yaw, 0.0f);

	const FVector ForwardDirection = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, InMovementVector.X);
	AddMovementInput(RightDirection, InMovementVector.Y);
}

void APS3PlayerCharacter::Look(const FVector2D& InLookVector)
{
	if (!IsValid(Controller) || !CanUseFieldControls()) return;

	if (IsLocallyControlled())
	{
		AddControllerYawInput(InLookVector.X);
		AddControllerPitchInput(InLookVector.Y);
	}
}

void APS3PlayerCharacter::StartJump()
{
	if (CanUseFieldControls())
	{
		Jump();
	}
}

void APS3PlayerCharacter::StopJump()
{
	StopJumping();
}

void APS3PlayerCharacter::TryDropHeldObject()
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

void APS3PlayerCharacter::TryInteract()
{
	if (!IsLocallyControlled() || !CanUseFieldControls())
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
