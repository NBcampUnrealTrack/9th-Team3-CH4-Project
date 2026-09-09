#include "Component/CosmeticComponent.h"

#include "Component/InteractionSwitchComponent.h"
#include "Component/OverlapSwitchComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/TimelineComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Object/ControlDoor.h"
#include "Object/Jeoul.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"

UCosmeticComponent::UCosmeticComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

void UCosmeticComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeEffect();
	InitializeDoorState();
	ApplyActiveState();
	BindActivationDelegates();
}

void UCosmeticComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ActiveDurationTimerHandle);
		World->GetTimerManager().ClearTimer(DoorTravelTimerHandle);
	}

	UnbindOwnerSwitchDelegates();
	UnbindOwnerJudgementDelegates();
	UnbindOwnerDoorDelegate();
	DestroyManagedComponents();

	Super::EndPlay(EndPlayReason);
}

void UCosmeticComponent::SetCosmeticActive(bool bActive)
{
	bIsCosmeticActive = bActive;
	ApplyActiveState();
}

void UCosmeticComponent::InitializeEffect()
{
	switch (EffectType)
	{
	case ECosmeticEffectType::BlueLight:
		CreateLightEffect(FLinearColor::Blue);
		break;
	case ECosmeticEffectType::RedLight:
		CreateLightEffect(FLinearColor::Red);
		break;
	case ECosmeticEffectType::ColorJudgement:
		CreateLightEffect(FLinearColor::White);
		break;
	case ECosmeticEffectType::Smoke:
		CreateSmokeEffect();
		break;
	case ECosmeticEffectType::None:
	default:
		break;
	}
}

void UCosmeticComponent::CreateLightEffect(const FLinearColor& LightColor)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	ManagedLightComponent = NewObject<UPointLightComponent>(Owner);
	if (!IsValid(ManagedLightComponent))
	{
		return;
	}

	ManagedLightComponent->SetupAttachment(this);
	ManagedLightComponent->SetRelativeLocation(FVector::ZeroVector);
	ManagedLightComponent->SetLightColor(LightColor);
	ManagedLightComponent->SetIntensity(LightIntensity);
	ManagedLightComponent->SetAttenuationRadius(LightAttenuationRadius);
	ManagedLightComponent->RegisterComponent();
	ManagedLightComponent->SetVisibility(false);
	ManagedLightComponent->SetActive(false);
}

void UCosmeticComponent::CreateSmokeEffect()
{
	if (!IsValid(SmokeTemplate))
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	ManagedParticleComponent = NewObject<UParticleSystemComponent>(Owner);
	if (!IsValid(ManagedParticleComponent))
	{
		return;
	}

	ManagedParticleComponent->SetupAttachment(this);
	ManagedParticleComponent->SetRelativeLocation(FVector::ZeroVector);
	ManagedParticleComponent->SetTemplate(SmokeTemplate);
	ManagedParticleComponent->bAutoActivate = false;
	ManagedParticleComponent->RegisterComponent();
	ManagedParticleComponent->SetVisibility(false);
	ManagedParticleComponent->DeactivateSystem();
}

void UCosmeticComponent::ApplyActiveState()
{
	if (IsValid(ManagedLightComponent))
	{
		ManagedLightComponent->SetVisibility(bIsCosmeticActive);
		ManagedLightComponent->SetActive(bIsCosmeticActive);
	}

	if (IsValid(ManagedParticleComponent))
	{
		ManagedParticleComponent->SetVisibility(bIsCosmeticActive);
		if (bIsCosmeticActive)
		{
			ManagedParticleComponent->ActivateSystem(true);
		}
		else
		{
			ManagedParticleComponent->DeactivateSystem();
		}
	}
}

void UCosmeticComponent::DestroyManagedComponents()
{
	if (IsValid(ManagedLightComponent))
	{
		ManagedLightComponent->DestroyComponent();
		ManagedLightComponent = nullptr;
	}

	if (IsValid(ManagedParticleComponent))
	{
		StopDoorOpacityTimeline();
		ManagedParticleComponent->DestroyComponent();
		ManagedParticleComponent = nullptr;
	}

	if (IsValid(DoorOpacityTimelineComponent))
	{
		DoorOpacityTimelineComponent->DestroyComponent();
		DoorOpacityTimelineComponent = nullptr;
	}

	DoorSmokeDynamicMaterial = nullptr;
}

void UCosmeticComponent::InitializeDoorState()
{
	if (ActivationType != ECosmeticActivationType::Door)
	{
		return;
	}

	DoorTravelDuration = FMath::Max(0.0f, DoorTravelDuration);
	DoorProgressTime = DoorTravelDuration;
	DoorTravelDirection = ECosmeticDoorTravelDirection::None;
	InitializeDoorOpacityTimeline();
	CreateDoorSmokeDynamicMaterial();
	ApplyDoorSmokeOpacity();

	if (const UWorld* World = GetWorld())
	{
		LastDoorStateChangeTime = World->GetTimeSeconds();
	}
}

void UCosmeticComponent::InitializeDoorOpacityTimeline()
{
	if (EffectType != ECosmeticEffectType::Smoke)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	DoorOpacityTimelineComponent = NewObject<UTimelineComponent>(Owner);
	if (!IsValid(DoorOpacityTimelineComponent))
	{
		return;
	}

	DoorOpacityTimelineComponent->SetPropertySetObject(this);
	DoorOpacityTimelineComponent->SetLooping(true);
	DoorOpacityTimelineComponent->SetTimelineLength(1.0f);

	FOnTimelineEvent OnTimelineUpdate;
	OnTimelineUpdate.BindUFunction(this, FName("HandleDoorOpacityTimelineUpdate"));
	DoorOpacityTimelineComponent->SetTimelinePostUpdateFunc(OnTimelineUpdate);
	DoorOpacityTimelineComponent->RegisterComponent();
}

void UCosmeticComponent::CreateDoorSmokeDynamicMaterial()
{
	if (!IsValid(ManagedParticleComponent))
	{
		return;
	}

	DoorSmokeDynamicMaterial = ManagedParticleComponent->CreateDynamicMaterialInstance(0);
}

void UCosmeticComponent::BindActivationDelegates()
{
	switch (ActivationType)
	{
	case ECosmeticActivationType::Toggle:
	case ECosmeticActivationType::Timed:
		if (EffectType != ECosmeticEffectType::ColorJudgement)
		{
			BindOwnerSwitchDelegates();
		}
		break;
	case ECosmeticActivationType::JudgementToggle:
	case ECosmeticActivationType::JudgementTimed:
		if (EffectType == ECosmeticEffectType::ColorJudgement)
		{
			BindOwnerJudgementDelegates();
		}
		break;
	case ECosmeticActivationType::Door:
		if (EffectType == ECosmeticEffectType::Smoke)
		{
			BindOwnerDoorDelegate();
		}
		break;
	default:
		break;
	}
}

void UCosmeticComponent::BindOwnerSwitchDelegates()
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	TArray<UInteractionSwitchComponent*> InteractionSwitchComponents;
	Owner->GetComponents<UInteractionSwitchComponent>(InteractionSwitchComponents);
	for (UInteractionSwitchComponent* SwitchComponent : InteractionSwitchComponents)
	{
		if (!IsValid(SwitchComponent) || SwitchComponent->GetOwner() != Owner)
		{
			continue;
		}

		if (ActivationType == ECosmeticActivationType::Toggle)
		{
			SwitchComponent->OnSwitchActivatedChanged.AddUObject(
				this,
				&UCosmeticComponent::HandleToggleActivationChanged);
		}
		else
		{
			SwitchComponent->OnInteractionSuccessed.AddUObject(
				this,
				&UCosmeticComponent::HandleTimedInteractionSucceeded);
		}

		BoundInteractionSwitchComponents.Add(SwitchComponent);
	}

	TArray<UOverlapSwitchComponent*> OverlapSwitchComponents;
	Owner->GetComponents<UOverlapSwitchComponent>(OverlapSwitchComponents);
	for (UOverlapSwitchComponent* SwitchComponent : OverlapSwitchComponents)
	{
		if (!IsValid(SwitchComponent) || SwitchComponent->GetOwner() != Owner)
		{
			continue;
		}

		if (ActivationType == ECosmeticActivationType::Toggle)
		{
			SwitchComponent->OnOverlapStateChanged.AddUObject(
				this,
				&UCosmeticComponent::HandleToggleActivationChanged);
		}
		else
		{
			SwitchComponent->OnOverlapStateChanged.AddUObject(
				this,
				&UCosmeticComponent::HandleTimedOverlapStateChanged);
		}

		BoundOverlapSwitchComponents.Add(SwitchComponent);
	}
}

void UCosmeticComponent::BindOwnerJudgementDelegates()
{
	AJeoul* JeoulOwner = Cast<AJeoul>(GetOwner());
	if (!IsValid(JeoulOwner))
	{
		return;
	}

	JeoulOwner->OnJeoulCheckFinished.AddUObject(
		this,
		&UCosmeticComponent::HandleJudgementFinished);

	BoundJeoulOwner = JeoulOwner;
}

void UCosmeticComponent::BindOwnerDoorDelegate()
{
	AControlDoor* ControlDoorOwner = Cast<AControlDoor>(GetOwner());
	if (!IsValid(ControlDoorOwner))
	{
		return;
	}

	ControlDoorOwner->OnIsControlDoorOpen.AddUObject(
		this,
		&UCosmeticComponent::HandleDoorOpenStateChanged);

	BoundControlDoorOwner = ControlDoorOwner;
}

void UCosmeticComponent::UnbindOwnerSwitchDelegates()
{
	for (const TWeakObjectPtr<UInteractionSwitchComponent>& SwitchComponent : BoundInteractionSwitchComponents)
	{
		if (SwitchComponent.IsValid())
		{
			SwitchComponent->OnSwitchActivatedChanged.RemoveAll(this);
			SwitchComponent->OnInteractionSuccessed.RemoveAll(this);
		}
	}
	BoundInteractionSwitchComponents.Empty();

	for (const TWeakObjectPtr<UOverlapSwitchComponent>& SwitchComponent : BoundOverlapSwitchComponents)
	{
		if (SwitchComponent.IsValid())
		{
			SwitchComponent->OnOverlapStateChanged.RemoveAll(this);
		}
	}
	BoundOverlapSwitchComponents.Empty();
}

void UCosmeticComponent::UnbindOwnerJudgementDelegates()
{
	if (BoundJeoulOwner.IsValid())
	{
		BoundJeoulOwner->OnJeoulCheckFinished.RemoveAll(this);
		BoundJeoulOwner = nullptr;
	}
}

void UCosmeticComponent::UnbindOwnerDoorDelegate()
{
	if (BoundControlDoorOwner.IsValid())
	{
		BoundControlDoorOwner->OnIsControlDoorOpen.RemoveAll(this);
		BoundControlDoorOwner = nullptr;
	}
}

void UCosmeticComponent::HandleToggleActivationChanged(bool bActive)
{
	SetCosmeticActive(bActive);
}

void UCosmeticComponent::HandleTimedInteractionSucceeded()
{
	StartTimedActivation();
}

void UCosmeticComponent::HandleTimedOverlapStateChanged(bool bOverlapped)
{
	if (bOverlapped)
	{
		StartTimedActivation();
	}
}

void UCosmeticComponent::HandleJudgementFinished(bool bIsSuccess)
{
	if (!IsValid(ManagedLightComponent))
	{
		return;
	}

	ManagedLightComponent->SetLightColor(
		bIsSuccess ? FLinearColor::Blue : FLinearColor::Red);

	if (ActivationType == ECosmeticActivationType::JudgementTimed)
	{
		StartTimedActivation();
		return;
	}

	SetCosmeticActive(true);
}

void UCosmeticComponent::HandleDoorOpenStateChanged(bool bIsOpen)
{
	DoorTravelDuration = FMath::Max(0.0f, DoorTravelDuration);
	UpdateDoorProgressToNow();

	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		World->GetTimerManager().ClearTimer(DoorTravelTimerHandle);
	}
	StopDoorOpacityTimeline();

	const bool bOpening = bIsOpen;
	if (DoorTravelDuration <= KINDA_SMALL_NUMBER)
	{
		FinishDoorTravelImmediately(bOpening);
		return;
	}

	const float RemainingTime = bOpening
		? DoorProgressTime
		: DoorTravelDuration - DoorProgressTime;

	if (World)
	{
		LastDoorStateChangeTime = World->GetTimeSeconds();
	}

	if (RemainingTime <= KINDA_SMALL_NUMBER)
	{
		FinishDoorTravelImmediately(bOpening);
		return;
	}

	DoorTravelDirection = bOpening
		? ECosmeticDoorTravelDirection::Opening
		: ECosmeticDoorTravelDirection::Closing;

	if (!bIsCosmeticActive)
	{
		SetCosmeticActive(true);
	}
	ApplyDoorSmokeOpacity();
	StartDoorOpacityTimeline();

	if (World)
	{
		World->GetTimerManager().SetTimer(
			DoorTravelTimerHandle,
			this,
			&UCosmeticComponent::FinishDoorTravel,
			RemainingTime,
			false);
	}
	else
	{
		FinishDoorTravelImmediately(bOpening);
	}
}

void UCosmeticComponent::UpdateDoorProgressToNow()
{
	const float ClampedDoorTravelDuration = FMath::Max(0.0f, DoorTravelDuration);
	const UWorld* World = GetWorld();
	if (!IsValid(World) || DoorTravelDirection == ECosmeticDoorTravelDirection::None)
	{
		DoorProgressTime = FMath::Clamp(DoorProgressTime, 0.0f, ClampedDoorTravelDuration);
		return;
	}

	const double CurrentTime = World->GetTimeSeconds();
	const float ElapsedTime = static_cast<float>(FMath::Max(0.0, CurrentTime - LastDoorStateChangeTime));
	LastDoorStateChangeTime = CurrentTime;

	if (DoorTravelDirection == ECosmeticDoorTravelDirection::Opening)
	{
		DoorProgressTime -= ElapsedTime;
	}
	else if (DoorTravelDirection == ECosmeticDoorTravelDirection::Closing)
	{
		DoorProgressTime += ElapsedTime;
	}

	DoorProgressTime = FMath::Clamp(DoorProgressTime, 0.0f, ClampedDoorTravelDuration);
}

void UCosmeticComponent::HandleDoorOpacityTimelineUpdate()
{
	UpdateDoorProgressToNow();
	ApplyDoorSmokeOpacity();
}

void UCosmeticComponent::StartDoorOpacityTimeline()
{
	if (!IsValid(DoorOpacityTimelineComponent) || !IsValid(DoorSmokeDynamicMaterial))
	{
		return;
	}

	DoorOpacityTimelineComponent->PlayFromStart();
}

void UCosmeticComponent::StopDoorOpacityTimeline()
{
	if (IsValid(DoorOpacityTimelineComponent))
	{
		DoorOpacityTimelineComponent->Stop();
	}
}

void UCosmeticComponent::ApplyDoorSmokeOpacity()
{
	if (!IsValid(DoorSmokeDynamicMaterial))
	{
		return;
	}

	float Opacity = 0.0f;
	if (DoorTravelDuration > KINDA_SMALL_NUMBER)
	{
		const float NormalizedDoorProgress = FMath::Clamp(DoorProgressTime / DoorTravelDuration, 0.0f, 1.0f);
		Opacity = 1.0f - FMath::Abs(NormalizedDoorProgress * 2.0f - 1.0f);
	}

	DoorSmokeDynamicMaterial->SetScalarParameterValue(SmokeOpacityParameterName, Opacity);
}

void UCosmeticComponent::FinishDoorTravel()
{
	if (DoorTravelDirection == ECosmeticDoorTravelDirection::None)
	{
		return;
	}

	if (DoorTravelDirection == ECosmeticDoorTravelDirection::Opening)
	{
		DoorProgressTime = 0.0f;
	}
	else if (DoorTravelDirection == ECosmeticDoorTravelDirection::Closing)
	{
		DoorProgressTime = FMath::Max(0.0f, DoorTravelDuration);
	}

	StopDoorOpacityTimeline();
	ApplyDoorSmokeOpacity();
	DoorTravelDirection = ECosmeticDoorTravelDirection::None;

	if (const UWorld* World = GetWorld())
	{
		LastDoorStateChangeTime = World->GetTimeSeconds();
	}

	SetCosmeticActive(false);
}

void UCosmeticComponent::FinishDoorTravelImmediately(bool bOpening)
{
	DoorProgressTime = bOpening ? 0.0f : FMath::Max(0.0f, DoorTravelDuration);
	StopDoorOpacityTimeline();
	ApplyDoorSmokeOpacity();
	DoorTravelDirection = ECosmeticDoorTravelDirection::None;

	if (const UWorld* World = GetWorld())
	{
		LastDoorStateChangeTime = World->GetTimeSeconds();
	}

	SetCosmeticActive(false);
}

void UCosmeticComponent::StartTimedActivation()
{
	SetCosmeticActive(true);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ActiveDurationTimerHandle);

		if (ActiveDuration > 0.0f)
		{
			World->GetTimerManager().SetTimer(
				ActiveDurationTimerHandle,
				this,
				&UCosmeticComponent::FinishTimedActivation,
				ActiveDuration,
				false);
			return;
		}
	}

	FinishTimedActivation();
}

void UCosmeticComponent::FinishTimedActivation()
{
	SetCosmeticActive(false);
}
