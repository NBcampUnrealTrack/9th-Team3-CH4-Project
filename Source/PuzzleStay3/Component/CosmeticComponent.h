#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Data/Enum/CosmeticActivationType.h"
#include "Data/Enum/CosmeticEffectType.h"
#include "CosmeticComponent.generated.h"

class AJeoul;
class AControlDoor;
class UInteractionSwitchComponent;
class UOverlapSwitchComponent;
class UParticleSystem;
class UParticleSystemComponent;
class UPointLightComponent;
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
	ECosmeticActivationType ActivationType = ECosmeticActivationType::Toggle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic",
		meta = (EditCondition = "ActivationType == ECosmeticActivationType::Timed || ActivationType == ECosmeticActivationType::JudgementTimed", ClampMin = "0.0"))
	float ActiveDuration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic|Door",
		meta = (EditCondition = "ActivationType == ECosmeticActivationType::Door", ClampMin = "0.0"))
	float DoorTravelDuration = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic|Door",
		meta = (EditCondition = "ActivationType == ECosmeticActivationType::Door"))
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
	void CreateDoorSmokeDynamicMaterial();
	void BindActivationDelegates();
	void BindOwnerSwitchDelegates();
	void BindOwnerJudgementDelegates();
	void BindOwnerDoorDelegate();
	void UnbindOwnerSwitchDelegates();
	void UnbindOwnerJudgementDelegates();
	void UnbindOwnerDoorDelegate();
	void HandleToggleActivationChanged(bool bActive);
	void HandleTimedInteractionSucceeded();
	void HandleTimedOverlapStateChanged(bool bOverlapped);
	void HandleJudgementFinished(bool bIsSuccess);
	void HandleDoorOpenStateChanged(bool bIsOpen);
	void UpdateDoorProgressToNow();
	UFUNCTION()
	void HandleDoorOpacityTimelineUpdate();
	void StartDoorOpacityTimeline();
	void StopDoorOpacityTimeline();
	void ApplyDoorSmokeOpacity();
	void FinishDoorTravel();
	void FinishDoorTravelImmediately(bool bOpening);
	void StartTimedActivation();
	void FinishTimedActivation();

	UPROPERTY(Transient)
	TObjectPtr<UPointLightComponent> ManagedLightComponent;

	UPROPERTY(Transient)
	TObjectPtr<UParticleSystemComponent> ManagedParticleComponent;

	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<UInteractionSwitchComponent>> BoundInteractionSwitchComponents;

	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<UOverlapSwitchComponent>> BoundOverlapSwitchComponents;

	UPROPERTY(Transient)
	TWeakObjectPtr<AJeoul> BoundJeoulOwner;

	UPROPERTY(Transient)
	TWeakObjectPtr<AControlDoor> BoundControlDoorOwner;

	UPROPERTY(Transient)
	TObjectPtr<UTimelineComponent> DoorOpacityTimelineComponent;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> DoorSmokeDynamicMaterial;

	FTimerHandle ActiveDurationTimerHandle;
	FTimerHandle DoorTravelTimerHandle;
	float DoorProgressTime = 0.0f;
	double LastDoorStateChangeTime = 0.0;
	ECosmeticDoorTravelDirection DoorTravelDirection = ECosmeticDoorTravelDirection::None;
};
