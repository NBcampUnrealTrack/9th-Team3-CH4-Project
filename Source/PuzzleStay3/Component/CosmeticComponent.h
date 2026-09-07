#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Data/Enum/CosmeticEffectType.h"
#include "CosmeticComponent.generated.h"

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
	void StartTestToggleTimer();
	void HandleTestToggleTimer();

	UPROPERTY(Transient)
	TObjectPtr<UPointLightComponent> ManagedLightComponent;

	UPROPERTY(Transient)
	TObjectPtr<UParticleSystemComponent> ManagedParticleComponent;

	FTimerHandle TestToggleTimerHandle;
};
