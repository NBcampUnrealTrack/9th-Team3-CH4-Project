#include "Component/CosmeticComponent.h"

#include "Components/PointLightComponent.h"
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

	// Test code: toggles the cosmetic effect every 5 seconds for temporary behavior checks.
	StartTestToggleTimer();
}

void UCosmeticComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TestToggleTimerHandle);
	}

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

void UCosmeticComponent::StartTestToggleTimer()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			TestToggleTimerHandle,
			this,
			&UCosmeticComponent::HandleTestToggleTimer,
			5.0f,
			true);
	}
}

void UCosmeticComponent::HandleTestToggleTimer()
{
	SetCosmeticActive(!bIsCosmeticActive);
}
