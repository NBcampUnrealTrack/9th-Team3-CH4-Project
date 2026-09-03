
#include "PS3PlayerCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/Controller/PS3PlayerController.h"
#include "Component/InteractionSwitchComponent.h"
#include "Object/Dumbbell.h"
#include "Components/SceneComponent.h"


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
	
	CarryAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("CarryAnchor"));
	CarryAnchor->SetupAttachment(GetMesh());
}

bool APS3PlayerCharacter::CanUseFieldControls() const
{
	return IsValid(Cast<APS3PlayerController>(GetController()));
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

	if (!IsValid(HeldDumbbell))
	{
		HeldDumbbell = nullptr;
		return;
	}

	ADumbbell* DumbbellToDrop = HeldDumbbell;

	if (DumbbellToDrop->TryDrop(this))
	{
		HeldDumbbell = nullptr;
	}
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

	const FVector TraceStart = GetPawnViewLocation();
	const FRotator TraceRotation = Controller->GetControlRotation();

	const FVector TraceEnd =
		TraceStart + TraceRotation.Vector() * InteractionDistance;

	FCollisionQueryParams QueryParams(
		SCENE_QUERY_STAT(PlayerInteractionTrace),
		false,
		this
	);

	FHitResult HitResult;
	const bool bHit = World->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		QueryParams
	);

	if (!bHit || !IsValid(HitResult.GetActor()))
	{
		return;
	}
	
	AActor* HitActor = HitResult.GetActor();
	if (!IsValid(HitActor))
	{
		return;
	}

	// 이미 덤벨을 들고 있으면 다른 덤벨을 집지 않음
	if (IsValid(HeldDumbbell))
	{
		return;
	}

	if (ADumbbell* HitDumbbell = Cast<ADumbbell>(HitActor))
	{
		// Dumbbell에서 서버 권한, 다른 플레이어가 들고 있는지 등을 검사
		if (HitDumbbell->TryInteract(this))
		{
			HeldDumbbell = HitDumbbell;
		}

		// 덤벨이었으면 성공 여부와 관계없이 상호작용 처리 종료
		return;
	}
	
	UInteractionSwitchComponent* InteractionComponent = HitActor->FindComponentByClass<UInteractionSwitchComponent>();

	if (!IsValid(InteractionComponent))
	{
		return;
	}

	InteractionComponent->TryInteract(this);
}
