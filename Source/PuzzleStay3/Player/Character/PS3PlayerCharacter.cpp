
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
#include "Player/PlayerState/PS3PlayerState.h"
#include "Components/ActorComponent.h"
#include "Engine/OverlapResult.h"



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

void APS3PlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	RefreshPlayerIdentityVisual();
}

void APS3PlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	RefreshPlayerIdentityVisual();
}

void APS3PlayerCharacter::RefreshPlayerIdentityVisual()
{
	const APS3PlayerState* PS3PlayerState = GetPlayerState<APS3PlayerState>();
	if (!IsValid(PS3PlayerState))
	{
		return;
	}

	BP_ApplyPlayerIdentityVisual(PS3PlayerState->GetPlayerIdentity());
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

	if (APS3PlayerState* PS3PlayerState = GetPlayerState<APS3PlayerState>())
	{
		PS3PlayerState->SetInteractionState(EInteractionState::IsNotInteracting);
	}

	if (APS3PlayerController* PS3PlayerController = Cast<APS3PlayerController>(GetController()))
	{
		PS3PlayerController->Client_PrepareForRespawn();
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
	
	const FVector TraceDirection = GetActorForwardVector();
	const FVector TraceStart = GetActorLocation() + TraceDirection * InteractionTraceStartOffset + FVector::UpVector * InteractionTraceHeight;
	const FVector TraceEnd = TraceStart + TraceDirection * InteractionDistance;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(PlayerInteractionTrace), false, this);

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);

	TArray<FOverlapResult> OverlapResults;
	World->OverlapMultiByObjectType(OverlapResults, TraceStart, FQuat::Identity, ObjectQueryParams, FCollisionShape::MakeSphere(InteractionDistance), QueryParams);

	OverlapResults.Sort([TraceStart](const FOverlapResult& A, const FOverlapResult& B)
	{
		const AActor* ActorA = A.GetActor();
		const AActor* ActorB = B.GetActor();
		const float DistanceA = IsValid(ActorA) ? FVector::DistSquared(TraceStart, ActorA->GetActorLocation()) : TNumericLimits<float>::Max();
		const float DistanceB = IsValid(ActorB) ? FVector::DistSquared(TraceStart, ActorB->GetActorLocation()) : TNumericLimits<float>::Max();
		return DistanceA < DistanceB;
	});

	UObject* InteractableTarget = nullptr;
	ADumbbell* HitDumbbell = nullptr;
	FVector ImpactPoint = TraceEnd;
	TSet<TObjectPtr<AActor>> CheckedActors;
	const float MinimumConeDot = FMath::Cos(FMath::DegreesToRadians(InteractionConeHalfAngle));

	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		AActor* HitActor = OverlapResult.GetActor();
		if (!IsValid(HitActor) || CheckedActors.Contains(HitActor))
		{
			continue;
		}
		CheckedActors.Add(HitActor);

		const FVector ToCandidate = HitActor->GetActorLocation() - TraceStart;
		const float CandidateDistance = ToCandidate.Size();
		if (CandidateDistance <= UE_KINDA_SMALL_NUMBER || CandidateDistance > InteractionDistance || FVector::DotProduct(TraceDirection, ToCandidate / CandidateDistance) < MinimumConeDot)
		{
			continue;
		}

		ADumbbell* CandidateDumbbell = Cast<ADumbbell>(HitActor);
		if (IsValid(CandidateDumbbell) && IsValid(HeldDumbbell))
		{
			continue;
		}

		UObject* Candidate = FindInteractableTarget(HitActor);
		if (!IsValid(Candidate) || !IPS3InteractableInterface::Execute_CanInteract(Candidate, this))
		{
			continue;
		}

		FHitResult VisibilityHit;
		if (World->LineTraceSingleByChannel(VisibilityHit, TraceStart, HitActor->GetActorLocation(), ECC_Visibility, QueryParams) && VisibilityHit.GetActor() != HitActor)
		{
			continue;
		}

		InteractableTarget = Candidate;
		HitDumbbell = CandidateDumbbell;
		ImpactPoint = HitActor->GetActorLocation();
		break;
	}

	const bool bHit = IsValid(InteractableTarget);

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (bDrawInteractionTrace)
	{
		Client_DrawInteractionTrace(TraceStart, TraceEnd, bHit, ImpactPoint);
	}
#endif
	
	if (!IsValid(InteractableTarget))
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
	
	const FVector Direction = (TraceEnd - TraceStart).GetSafeNormal();
	const float ConeAngleRadians = FMath::DegreesToRadians(InteractionConeHalfAngle);
	DrawDebugCone(World, TraceStart, Direction, FVector::Distance(TraceStart, TraceEnd), ConeAngleRadians, ConeAngleRadians, 24, bHit ? FColor::Green : FColor::Red, false, InteractionTraceDebugDuration, 0, 1.5f);

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
