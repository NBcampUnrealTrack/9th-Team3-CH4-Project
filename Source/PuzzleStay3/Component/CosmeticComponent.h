#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Data/Enum/CosmeticActivationType.h"
#include "Data/Enum/CosmeticEffectType.h"
#include "CosmeticComponent.generated.h"

class UInteractionSwitchComponent;
class UOverlapSwitchComponent;
class UParticleSystem;
class UParticleSystemComponent;
class UPointLightComponent;

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
		meta = (EditCondition = "ActivationType == ECosmeticActivationType::Timed", ClampMin = "0.0"))
	float ActiveDuration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic|Light",
		meta = (EditCondition = "EffectType == ECosmeticEffectType::BlueLight || EffectType == ECosmeticEffectType::RedLight", ClampMin = "0.0"))
	float LightIntensity = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic|Light",
		meta = (EditCondition = "EffectType == ECosmeticEffectType::BlueLight || EffectType == ECosmeticEffectType::RedLight", ClampMin = "0.0"))
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
	void BindOwnerSwitchDelegates();
	void UnbindOwnerSwitchDelegates();
	void HandleToggleActivationChanged(bool bActive);
	void HandleTimedInteractionSucceeded();
	void HandleTimedOverlapStateChanged(bool bOverlapped);
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

	FTimerHandle ActiveDurationTimerHandle;
};
