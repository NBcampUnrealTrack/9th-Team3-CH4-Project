#include "Component/CosmeticComponent.h"

#include "Component/InteractionSwitchComponent.h"
#include "Component/OverlapSwitchComponent.h"
#include "Components/PointLightComponent.h"
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
	ApplyActiveState();
	BindActivationDelegates();
}

void UCosmeticComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ActiveDurationTimerHandle);
	}

	UnbindOwnerSwitchDelegates();
	UnbindOwnerJudgementDelegates();
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
		ManagedParticleComponent->DestroyComponent();
		ManagedParticleComponent = nullptr;
	}
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
