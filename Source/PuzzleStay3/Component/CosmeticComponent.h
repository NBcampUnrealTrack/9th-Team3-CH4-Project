#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Data/Enum/CosmeticActivationType.h"
#include "Data/Enum/CosmeticEffectType.h"
#include "CosmeticComponent.generated.h"

class AJeoul;
class AControlDoor;
class ADoor;
class APawn;
class UFakeDeathTrapComponent;
class UInteractionSwitchComponent;
class UOverlapSwitchComponent;
class UParticleSystem;
class UParticleSystemComponent;
class UPointLightComponent;
class URandomCollisionTrapComponent;
class UTimelineComponent;
class UMaterialInstanceDynamic;

enum class ECosmeticDoorTravelDirection : uint8
{
	None,
	Opening,
	Closing
};

UCLASS(ClassGroup = (Gimmick), meta = (BlueprintSpawnableComponent))
class PUZZLESTAY3_API UCosmeticComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UCosmeticComponent();

	UFUNCTION(BlueprintCallable, Category = "Cosmetic")
	void SetCosmeticActive(bool bActive);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic")
	ECosmeticEffectType EffectType = ECosmeticEffectType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic")
	ECosmeticActivationType ActivationType = ECosmeticActivationType::SwitchToggle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic",
		meta = (EditCondition = "ActivationType == ECosmeticActivationType::SwitchTimed || ActivationType == ECosmeticActivationType::TrapTimed || ActivationType == ECosmeticActivationType::JudgementTimed", ClampMin = "0.0"))
	float ActiveDuration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic|Door",
		meta = (EditCondition = "ActivationType == ECosmeticActivationType::DoorProgress", ClampMin = "0.0"))
	float DoorTravelDuration = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic|Smoke",
		meta = (EditCondition = "EffectType == ECosmeticEffectType::Smoke"))
	FName SmokeOpacityParameterName = TEXT("OpacityScale");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic|Light",
		meta = (EditCondition = "EffectType == ECosmeticEffectType::BlueLight || EffectType == ECosmeticEffectType::RedLight || EffectType == ECosmeticEffectType::ColorJudgement", ClampMin = "0.0"))
	float LightIntensity = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic|Light",
		meta = (EditCondition = "EffectType == ECosmeticEffectType::BlueLight || EffectType == ECosmeticEffectType::RedLight || EffectType == ECosmeticEffectType::ColorJudgement", ClampMin = "0.0"))
	float LightAttenuationRadius = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic|Smoke",
		meta = (EditCondition = "EffectType == ECosmeticEffectType::Smoke"))
	TObjectPtr<UParticleSystem> SmokeTemplate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cosmetic")
	bool bIsCosmeticActive = false;

private:
	void InitializeEffect();
	void CreateLightEffect(const FLinearColor& LightColor);
	void CreateSmokeEffect();
	void ApplyActiveState();
	void DestroyManagedComponents();
	void InitializeDoorState();
	void InitializeDoorOpacityTimeline();
	void CreateSmokeDynamicMaterial();
	void BindActivationDelegates();
	void BindOwnerSwitchDelegates();
	void BindOwnerTrapDelegates();
	void BindOwnerJudgementDelegates();
	void BindOwnerDoorDelegate();
	void UnbindOwnerSwitchDelegates();
	void UnbindOwnerTrapDelegates();
	void UnbindOwnerJudgementDelegates();
	void UnbindOwnerDoorDelegate();
	void HandleToggleActivationChanged(bool bActive);
	void HandleTimedInteractionSucceeded();
	void HandleTimedOverlapStateChanged(bool bOverlapped);
	UFUNCTION()
	void HandleFakeDeathTrapOverlapped(APawn* PlayerPawn);
	UFUNCTION()
	void HandleFakePlatformOverlapped(APawn* PlayerPawn);
	void HandleJudgementFinished(bool bIsSuccess);
	void HandleDoorOpenStateChanged(bool bIsOpen);
	void UpdateDoorProgressToNow();
	UFUNCTION()
	void HandleDoorOpacityTimelineUpdate();
	void StartDoorOpacityTimeline();
	void StopDoorOpacityTimeline();
	void ApplyDoorProgressOpacity();
	void SetSmokeOpacity(float Opacity);
	void FinishDoorTravel();
	void FinishDoorTravelImmediately(bool bOpening);
	void StartTimedActivation();
	void FinishTimedActivation();
	void StartTrapTimedActivation();
	void UpdateTrapTimedOpacity();
	void FinishTrapTimedActivation();

	UPROPERTY(Transient)
	TObjectPtr<UPointLightComponent> ManagedLightComponent;

	UPROPERTY(Transient)
	TObjectPtr<UParticleSystemComponent> ManagedParticleComponent;

	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<UInteractionSwitchComponent>> BoundInteractionSwitchComponents;

	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<UOverlapSwitchComponent>> BoundOverlapSwitchComponents;

	UPROPERTY(Transient)
	TWeakObjectPtr<UFakeDeathTrapComponent> BoundFakeDeathTrapComponent;

	UPROPERTY(Transient)
	TWeakObjectPtr<URandomCollisionTrapComponent> BoundRandomCollisionTrapComponent;

	UPROPERTY(Transient)
	TWeakObjectPtr<AJeoul> BoundJeoulOwner;

	UPROPERTY(Transient)
	TWeakObjectPtr<AControlDoor> BoundControlDoorOwner;

	UPROPERTY(Transient)
	TWeakObjectPtr<ADoor> BoundDoorOwner;

	UPROPERTY(Transient)
	TObjectPtr<UTimelineComponent> DoorOpacityTimelineComponent;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> SmokeDynamicMaterial;

	FTimerHandle ActiveDurationTimerHandle;
	FTimerHandle DoorTravelTimerHandle;
	FTimerHandle TrapTimedDurationTimerHandle;
	FTimerHandle TrapTimedOpacityTimerHandle;
	float DoorProgressTime = 0.0f;
	double TrapTimedStartTime = 0.0;
	double LastDoorStateChangeTime = 0.0;
	bool bIsTrapTimedActive = false;
	bool bHasCompletedTrapTimed = false;
	bool bHasHiddenTrapMesh = false;
	ECosmeticDoorTravelDirection DoorTravelDirection = ECosmeticDoorTravelDirection::None;
};
