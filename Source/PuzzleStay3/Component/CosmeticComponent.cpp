#include "Component/CosmeticComponent.h"

#include "Component/FakeDeathTrapComponent.h"
#include "Component/InteractionSwitchComponent.h"
#include "Component/OverlapSwitchComponent.h"
#include "Component/RandomCollisionTrapComponent.h"
#include "Component/S5_InteractionGimmickComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/TimelineComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Object/ControlDoor.h"
#include "Object/Door.h"
#include "Object/GimmickBase.h"
#include "Object/Jeoul.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h" // 현준 수정
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
		World->GetTimerManager().ClearTimer(TrapTimedDurationTimerHandle);
		World->GetTimerManager().ClearTimer(TrapTimedOpacityTimerHandle);
	}

	UnbindOwnerSwitchDelegates();
	UnbindOwnerTrapDelegates();
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
	CreateSmokeDynamicMaterial();
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

	SmokeDynamicMaterial = nullptr;
}

void UCosmeticComponent::InitializeDoorState()
{
	if (ActivationType != ECosmeticActivationType::DoorProgress)
	{
		return;
	}

	DoorTravelDuration = FMath::Max(0.0f, DoorTravelDuration);
	DoorProgressTime = DoorTravelDuration;
	DoorTravelDirection = ECosmeticDoorTravelDirection::None;
	InitializeDoorOpacityTimeline();
	ApplyDoorProgressOpacity();

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

void UCosmeticComponent::CreateSmokeDynamicMaterial()
{
	if (!IsValid(ManagedParticleComponent))
	{
		return;
	}

	SmokeDynamicMaterial = ManagedParticleComponent->CreateDynamicMaterialInstance(0);
}

void UCosmeticComponent::BindActivationDelegates()
{
	switch (ActivationType)
	{
	case ECosmeticActivationType::SwitchToggle:
	case ECosmeticActivationType::SwitchOn:
	case ECosmeticActivationType::SwitchTimed:
		if (EffectType != ECosmeticEffectType::ColorJudgement)
		{
			BindOwnerSwitchDelegates();
		}
		break;
	case ECosmeticActivationType::TrapTimed:
		BindOwnerTrapDelegates();
		break;
	case ECosmeticActivationType::JudgementPersistent:
	case ECosmeticActivationType::JudgementTimed:
		if (EffectType == ECosmeticEffectType::ColorJudgement)
		{
			BindOwnerJudgementDelegates();
		}
		break;
	case ECosmeticActivationType::DoorProgress:
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

		switch (ActivationType)
		{
		case ECosmeticActivationType::SwitchToggle:
			SwitchComponent->OnSwitchActivatedChanged.AddUObject(
				this,
				&UCosmeticComponent::HandleToggleActivationChanged);
			break;
		case ECosmeticActivationType::SwitchOn:
			SwitchComponent->OnCosmeticInteractionSuccessed.AddUObject(
				this,
				&UCosmeticComponent::HandleSwitchOnInteractionSucceeded);
			break;
		case ECosmeticActivationType::SwitchTimed:
			SwitchComponent->OnCosmeticInteractionSuccessed.AddUObject(
				this,
				&UCosmeticComponent::HandleTimedInteractionSucceeded);
			break;
		default:
			break;
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

		if (ActivationType == ECosmeticActivationType::SwitchToggle)
		{
			SwitchComponent->OnOverlapStateChanged.AddUObject(
				this,
				&UCosmeticComponent::HandleToggleActivationChanged);
		}
		else if (ActivationType == ECosmeticActivationType::SwitchTimed)
		{
			SwitchComponent->OnOverlapStateChanged.AddUObject(
				this,
				&UCosmeticComponent::HandleTimedOverlapStateChanged);
		}

		BoundOverlapSwitchComponents.Add(SwitchComponent);
	}

	TArray<US5_InteractionGimmickComponent*> S5InteractionGimmickComponents;
	Owner->GetComponents<US5_InteractionGimmickComponent>(S5InteractionGimmickComponents);
	for (US5_InteractionGimmickComponent* S5InteractionGimmickComponent : S5InteractionGimmickComponents)
	{
		if (!IsValid(S5InteractionGimmickComponent) || S5InteractionGimmickComponent->GetOwner() != Owner)
		{
			continue;
		}

		if (ActivationType == ECosmeticActivationType::SwitchOn)
		{
			S5InteractionGimmickComponent->OnCosmeticInteractionSuccessed.AddUObject(
				this,
				&UCosmeticComponent::HandleSwitchOnInteractionSucceeded);
		}
		else if (ActivationType == ECosmeticActivationType::SwitchTimed)
		{
			S5InteractionGimmickComponent->OnCosmeticInteractionSuccessed.AddUObject(
				this,
				&UCosmeticComponent::HandleTimedInteractionSucceeded);
		}
		else
		{
			continue;
		}

		BoundS5InteractionGimmickComponents.Add(S5InteractionGimmickComponent);
	}
}

void UCosmeticComponent::BindOwnerTrapDelegates()
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	UFakeDeathTrapComponent* FakeDeathTrapComponent = Owner->FindComponentByClass<UFakeDeathTrapComponent>();
	if (IsValid(FakeDeathTrapComponent) && FakeDeathTrapComponent->GetOwner() == Owner)
	{
		FakeDeathTrapComponent->OnFakeDeathTrapOverlapped.AddUniqueDynamic(
			this,
			&UCosmeticComponent::HandleFakeDeathTrapOverlapped);
		BoundFakeDeathTrapComponent = FakeDeathTrapComponent;
	}

	URandomCollisionTrapComponent* RandomCollisionTrapComponent = Owner->FindComponentByClass<URandomCollisionTrapComponent>();
	if (IsValid(RandomCollisionTrapComponent) && RandomCollisionTrapComponent->GetOwner() == Owner)
	{
		RandomCollisionTrapComponent->OnFakePlatformOverlapped.AddUniqueDynamic(
			this,
			&UCosmeticComponent::HandleFakePlatformOverlapped);
		BoundRandomCollisionTrapComponent = RandomCollisionTrapComponent;
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
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	AControlDoor* ControlDoorOwner = Cast<AControlDoor>(Owner);
	if (IsValid(ControlDoorOwner))
	{
		ControlDoorOwner->OnIsControlDoorOpen.AddUObject(
			this,
			&UCosmeticComponent::HandleDoorOpenStateChanged);

		BoundControlDoorOwner = ControlDoorOwner;
	}

	ADoor* DoorOwner = Cast<ADoor>(Owner);
	if (IsValid(DoorOwner))
	{
		DoorOwner->OnIsDoorOpen.AddUObject(
			this,
			&UCosmeticComponent::HandleDoorOpenStateChanged);
		DoorOwner->OnIsExitDoorOpen.AddUObject( // 현준 수정
			this, // 현준 수정
			&UCosmeticComponent::HandleExitDoorOpenStateChanged); // 현준 수정

		BoundDoorOwner = DoorOwner;
	}
}

void UCosmeticComponent::UnbindOwnerSwitchDelegates()
{
	for (const TWeakObjectPtr<UInteractionSwitchComponent>& SwitchComponent : BoundInteractionSwitchComponents)
	{
		if (SwitchComponent.IsValid())
		{
			SwitchComponent->OnSwitchActivatedChanged.RemoveAll(this);
			SwitchComponent->OnCosmeticInteractionSuccessed.RemoveAll(this);
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

	for (const TWeakObjectPtr<US5_InteractionGimmickComponent>& S5InteractionGimmickComponent : BoundS5InteractionGimmickComponents)
	{
		if (S5InteractionGimmickComponent.IsValid())
		{
			S5InteractionGimmickComponent->OnCosmeticInteractionSuccessed.RemoveAll(this);
		}
	}
	BoundS5InteractionGimmickComponents.Empty();
}

void UCosmeticComponent::UnbindOwnerTrapDelegates()
{
	if (BoundFakeDeathTrapComponent.IsValid())
	{
		BoundFakeDeathTrapComponent->OnFakeDeathTrapOverlapped.RemoveAll(this);
		BoundFakeDeathTrapComponent = nullptr;
	}

	if (BoundRandomCollisionTrapComponent.IsValid())
	{
		BoundRandomCollisionTrapComponent->OnFakePlatformOverlapped.RemoveAll(this);
		BoundRandomCollisionTrapComponent = nullptr;
	}
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

	if (BoundDoorOwner.IsValid())
	{
		BoundDoorOwner->OnIsDoorOpen.RemoveAll(this);
		BoundDoorOwner->OnIsExitDoorOpen.RemoveAll(this); // 현준 수정
		BoundDoorOwner = nullptr;
	}
}

void UCosmeticComponent::PlayInteractionSound() // 현준 수정
{ // 현준 수정
	if (GetNetMode() == NM_DedicatedServer) // 현준 수정
	{ // 현준 수정
		return; // 현준 수정
	} // 현준 수정

	if (!IsValid(InteractionSound)) // 현준 수정
	{ // 현준 수정
		return; // 현준 수정
	} // 현준 수정

	AActor* Owner = GetOwner(); // 현준 수정
	if (!IsValid(Owner)) // 현준 수정
	{ // 현준 수정
		return; // 현준 수정
	} // 현준 수정

	const UWorld* World = GetWorld(); // 현준 수정
	if (IsValid(World)) // 현준 수정
	{ // 현준 수정
		const double CurrentTime = World->GetTimeSeconds(); // 현준 수정
		if (FMath::IsNearlyEqual(CurrentTime, LastInteractionSoundPlayTime)) // 현준 수정
		{ // 현준 수정
			return; // 현준 수정
		} // 현준 수정

		LastInteractionSoundPlayTime = CurrentTime; // 현준 수정
	} // 현준 수정

	UGameplayStatics::PlaySoundAtLocation(this, InteractionSound, Owner->GetActorLocation()); // 현준 수정
} // 현준 수정

void UCosmeticComponent::HandleToggleActivationChanged(bool bActive)
{
	if (bActive) // 현준 수정
	{ // 현준 수정
		PlayInteractionSound(); // 현준 수정
	} // 현준 수정

	SetCosmeticActive(bActive);
}

void UCosmeticComponent::HandleSwitchOnInteractionSucceeded()
{
	PlayInteractionSound(); // 현준 수정
	SetCosmeticActive(true);
}

void UCosmeticComponent::HandleTimedInteractionSucceeded()
{
	PlayInteractionSound(); // 현준 수정
	StartTimedActivation();
}

void UCosmeticComponent::HandleTimedOverlapStateChanged(bool bOverlapped)
{
	if (bOverlapped)
	{
		StartTimedActivation();
	}
}

void UCosmeticComponent::HandleFakeDeathTrapOverlapped(APawn* PlayerPawn)
{
	StartTrapTimedActivation();
}

void UCosmeticComponent::HandleFakePlatformOverlapped(APawn* PlayerPawn)
{
	StartTrapTimedActivation();
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
	AActor* Owner = GetOwner(); // 현준 수정
	if (IsValid(Owner) && IsValid(Cast<ADoor>(Owner)) && GetNetMode() != NM_DedicatedServer) // 현준 수정
	{
		USoundBase* DoorSound = bIsOpen ? DoorOpenSound : DoorCloseSound; // 현준 수정
		if (IsValid(DoorSound)) // 현준 수정
		{
			UGameplayStatics::PlaySoundAtLocation(this, DoorSound, Owner->GetActorLocation()); // 현준 수정
		}
	}

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
	ApplyDoorProgressOpacity();
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

void UCosmeticComponent::HandleExitDoorOpenStateChanged(bool bIsOpen) // 현준 수정
{
	if (!bIsOpen) // 현준 수정
	{
		return; // 현준 수정
	}

	if (GetNetMode() == NM_DedicatedServer) // 현준 수정
	{
		return; // 현준 수정
	}

	if (!IsValid(StageClearSound)) // 현준 수정
	{
		return; // 현준 수정
	}

	UGameplayStatics::PlaySound2D(this, StageClearSound); // 현준 수정
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
	ApplyDoorProgressOpacity();
}

void UCosmeticComponent::StartDoorOpacityTimeline()
{
	if (!IsValid(DoorOpacityTimelineComponent) || !IsValid(SmokeDynamicMaterial))
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

void UCosmeticComponent::ApplyDoorProgressOpacity()
{
	float Opacity = 0.0f;
	if (DoorTravelDuration > KINDA_SMALL_NUMBER)
	{
		const float NormalizedDoorProgress = FMath::Clamp(DoorProgressTime / DoorTravelDuration, 0.0f, 1.0f);
		Opacity = 1.0f - FMath::Abs(NormalizedDoorProgress * 2.0f - 1.0f);
	}

	SetSmokeOpacity(Opacity);
}

void UCosmeticComponent::SetSmokeOpacity(float Opacity)
{
	if (!IsValid(SmokeDynamicMaterial))
	{
		return;
	}

	SmokeDynamicMaterial->SetScalarParameterValue(SmokeOpacityParameterName, Opacity);
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
	ApplyDoorProgressOpacity();
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
	ApplyDoorProgressOpacity();
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

void UCosmeticComponent::StartTrapTimedActivation()
{
	if (bIsTrapTimedActive || bHasCompletedTrapTimed)
	{
		return;
	}

	bIsTrapTimedActive = true;
	bHasHiddenTrapMesh = false;
	// 현준 수정: 트랩 Cosmetic 수신 즉시 Mesh 숨김
	if (AGimmickBase* GimmickOwner = Cast<AGimmickBase>(GetOwner()))
	{
		GimmickOwner->HideGimmickMesh();
	}
	bHasHiddenTrapMesh = true;

	SetCosmeticActive(true);

	UWorld* World = GetWorld();
	if (!IsValid(World) || ActiveDuration <= KINDA_SMALL_NUMBER)
	{
		FinishTrapTimedActivation();
		return;
	}

	TrapTimedStartTime = World->GetTimeSeconds();

	if (EffectType == ECosmeticEffectType::Smoke)
	{
		SetSmokeOpacity(0.0f);
		World->GetTimerManager().SetTimer(
			TrapTimedOpacityTimerHandle,
			this,
			&UCosmeticComponent::UpdateTrapTimedOpacity,
			1.0f / 30.0f,
			true);
	}

	World->GetTimerManager().SetTimer(
		TrapTimedDurationTimerHandle,
		this,
		&UCosmeticComponent::FinishTrapTimedActivation,
		ActiveDuration,
		false);
}

void UCosmeticComponent::UpdateTrapTimedOpacity()
{
	if (EffectType != ECosmeticEffectType::Smoke)
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (!IsValid(World) || ActiveDuration <= KINDA_SMALL_NUMBER)
	{
		SetSmokeOpacity(0.0f);
		return;
	}

	const float ElapsedTime = static_cast<float>(FMath::Max(0.0, World->GetTimeSeconds() - TrapTimedStartTime));
	const float NormalizedProgress = FMath::Clamp(ElapsedTime / ActiveDuration, 0.0f, 1.0f);
	const float Opacity = 1.0f - FMath::Abs(NormalizedProgress * 2.0f - 1.0f);
	SetSmokeOpacity(Opacity);

	if (!bHasHiddenTrapMesh && NormalizedProgress >= 0.5f)
	{
		if (AGimmickBase* GimmickOwner = Cast<AGimmickBase>(GetOwner()))
		{
			GimmickOwner->HideGimmickMesh();
		}

		bHasHiddenTrapMesh = true;
	}
}

void UCosmeticComponent::FinishTrapTimedActivation()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TrapTimedDurationTimerHandle);
		World->GetTimerManager().ClearTimer(TrapTimedOpacityTimerHandle);
	}

	if (EffectType == ECosmeticEffectType::Smoke)
	{
		SetSmokeOpacity(0.0f);
	}

	bIsTrapTimedActive = false;
	SetCosmeticActive(false);

	bHasCompletedTrapTimed = true;
}
