
#include "PS3PlayerCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/Controller/PS3PlayerController.h"
#include "Component/InteractionSwitchComponent.h"
#include "Object/Dumbbell.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"
#include "Player/Interaction/PS3InteractableInterface.h"
#include "Components/ActorComponent.h"



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
	CarryAnchor->SetupAttachment(GetMesh(), TEXT("hand_r"));
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

void APS3PlayerCharacter::PrepareForRespawn()
{
	if (!HasAuthority())
	{
		return;
	}

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->StopMovementImmediately();
		MovementComponent->DisableMovement();
	}

	if (IsValid(HeldDumbbell))
	{
		ADumbbell* DumbbellToDrop = HeldDumbbell;
		if (DumbbellToDrop->TryDrop(this))
		{
			HeldDumbbell = nullptr;
		}
	}

	SetActorEnableCollision(false);
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
	const FVector TraceEnd = TraceStart + Controller->GetControlRotation().Vector() * InteractionDistance;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(PlayerInteractionTrace), false, this);

	FHitResult HitResult;
	const bool bHit = World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams);

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (bDrawInteractionTrace)
	{
		Client_DrawInteractionTrace(TraceStart, TraceEnd, bHit, bHit ? HitResult.ImpactPoint : TraceEnd);
	}
#endif

	AActor* HitActor = HitResult.GetActor();

	if (!bHit || !IsValid(HitActor))
	{
		return;
	}

	ADumbbell* HitDumbbell = Cast<ADumbbell>(HitActor);
	if (IsValid(HitDumbbell) && IsValid(HeldDumbbell))
	{
		return;
	}

	UObject* InteractableTarget = FindInteractableTarget(HitActor);

	if (!IsValid(InteractableTarget))
	{
		return;
	}

	if (!IPS3InteractableInterface::Execute_CanInteract(InteractableTarget, this))
	{
		return;
	}

	const bool bSucceeded = IPS3InteractableInterface::Execute_Interact(InteractableTarget, this);

	if (!bSucceeded)
	{
		return;
	}

	// 임시 덤벨 호환 처리
	if (IsValid(HitDumbbell))
	{
		HeldDumbbell = HitDumbbell;
	}
}

void APS3PlayerCharacter::Client_DrawInteractionTrace_Implementation(const FVector TraceStart, const FVector TraceEnd, const bool bHit, const FVector ImpactPoint)
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	DrawDebugLine(World, TraceStart, TraceEnd, bHit ? FColor::Green : FColor::Red, false, InteractionTraceDebugDuration, 0, 2.0f);

	if (bHit)
	{
		DrawDebugSphere(World, ImpactPoint, 10.0f, 12, FColor::Yellow, false, InteractionTraceDebugDuration, 0, 1.5f);
	}
#endif
}

UObject* APS3PlayerCharacter::FindInteractableTarget(AActor* HitActor) const
{
	if (!IsValid(HitActor))
	{
		return nullptr;
	}

	// Actor가 직접 인터페이스를 구현한 경우
	if (HitActor->Implements<UPS3InteractableInterface>())
	{
		return HitActor;
	}

	// Actor에 붙은 Component가 인터페이스를 구현한 경우
	TInlineComponentArray<UActorComponent*> Components(HitActor);

	for (UActorComponent* Component : Components)
	{
		if (IsValid(Component) && Component->Implements<UPS3InteractableInterface>())
		{
			return Component;
		}
	}

	return nullptr;
}
