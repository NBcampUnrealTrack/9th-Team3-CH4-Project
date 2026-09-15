#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Data/Delegates/CosmeticDelegates.h"
#include "Player/Interaction/PS3InteractableInterface.h"
#include "S5_InteractionGimmickComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnInteractionGimmick_S5, bool);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PUZZLESTAY3_API US5_InteractionGimmickComponent : public UBoxComponent , public IPS3InteractableInterface
{
	GENERATED_BODY()

public:
	US5_InteractionGimmickComponent();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual bool CanInteract_Implementation(AActor* Requestor) const override;
	virtual bool Interact_Implementation(AActor* Requestor) override;
	
	void OnStartedGame(bool bIsGameStart);
	void OnColletedGimmickBase(const UBoxComponent* CurrentComponent, bool bIsInteractable);
	
	FOnInteractionGimmick_S5 OnInteractionGimmick;
	FOnCosmeticInteractionSuccessed OnCosmeticInteractionSuccessed;
	FOnInteractGimmickActivatedChanged_S5 OnSwitchActivatedChanged;
	
	UPROPERTY(Replicated)
	bool bIsInteractionGimmick = false;
	
	UPROPERTY(Replicated)
	bool bIsInteracted = false;
	
	UPROPERTY(Replicated)
	bool bIsStartedGame = false;

	// 현준 수정: Client Cosmetic 이벤트 전달용 RepNotify 시퀀스
	UPROPERTY(ReplicatedUsing = OnRep_CosmeticInteractionSuccessSequence)
	uint8 CosmeticInteractionSuccessSequence = 0;
	
	UPROPERTY(EditAnywhere, Category = "Interaction|Settings")
	float TargetFOVAngle = 120.0f;
	
protected:
	UFUNCTION()
	void OnCharacterBeginOverlapForUI(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnCharacterEndOverlapForUI(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	bool CheckCanDisplayedUI();
	void CheckCanDisplayedUIForTimer();

	// 현준 수정: Client Cosmetic 이벤트 전달용 RepNotify 콜백
	UFUNCTION()
	void OnRep_CosmeticInteractionSuccessSequence();
	
	UPROPERTY()
	TArray<TObjectPtr<AActor>> OverlappedCharacters;
	
	FTimerHandle CheckPlayerTimerHandle;
	
	bool bIsUIVisible = false;
	
	
};



