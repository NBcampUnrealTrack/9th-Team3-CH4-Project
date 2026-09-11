#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/Interaction/PS3InteractableInterface.h"
#include "Data/Delegates/CosmeticDelegates.h"
#include "InteractionSwitchComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSwitchActivatedChanged, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInteractionSuccessed, bool);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PUZZLESTAY3_API UInteractionSwitchComponent : public UActorComponent, public IPS3InteractableInterface
{
	GENERATED_BODY()

public:
	UInteractionSwitchComponent();

	virtual bool CanInteract_Implementation(AActor* Requestor) const override;
	virtual bool Interact_Implementation(AActor* Requestor) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	FOnSwitchActivatedChanged OnSwitchActivatedChanged;
	FOnInteractionSuccessed OnInteractionSuccessed;
	FOnCosmeticInteractionSuccessed OnCosmeticInteractionSuccessed;

	UFUNCTION(BlueprintCallable, Category = "Gimmick")
	bool IsActivated() const { return bIsActivated; }

	bool TryInteract(AActor* Requestor);
	void SetRegisterToGameMode(bool bRegister) { bRegisterToGameMode = bRegister; }

	void StartDisableTimer();
	void ResetSwitch();

	UFUNCTION(BlueprintCallable, Category = "Gimmick")
	void SetLocked(bool bNewLocked) { bIsLocked = bNewLocked; }

	AActor* GetInteractingActor() const { return InteractingActor; }

	UPROPERTY(Replicated)
	bool bIsEscapeDoor = true;
	
	UPROPERTY(Replicated)
	bool bIsInteractedGimmick = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick|Settings")
	int32 SwitchID = 1;
	
	// 스테이지 5 전용: true일 경우 내부 상태 로직을 수행하지 않고 브로드캐스트 후 종료
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Gimmick")
	bool bIsOtherInteractionGimmick = false;
	
	// true: 스위치를 켜도 플레이어 상태를 바꾸지 않음 (혼자서 4개 스위치 연속 조작 가능)
	// false: 스위치를 켜면 플레이어가 상호작용 중(IsInteracting) 상태가 됨 (다른 상호작용 불가)
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Gimmick")
	bool bMultiInteractionState = false; 

protected:
	// 램프/버튼의 활성화 상태
	UPROPERTY(ReplicatedUsing = OnRep_IsActivated)
	bool bIsActivated = false;

	// true일 경우 CanInteract가 항상 false를 반환
	UPROPERTY()
	bool bIsLocked = false;
	
	UPROPERTY(Replicated, Transient, VisibleAnywhere, Category = "Gimmick")
	TObjectPtr<AActor> InteractingActor = nullptr;

	// GameMode의 글로벌 퍼즐 스위치 목록에 등록할지 여부 일반 퍼즐 스위치는 true, 저울 버튼 등 독립 기믹 스위치는 false로 설정
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick")
	bool bRegisterToGameMode = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|Timer")
	bool bUseAutoDisableTimer = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|Timer",
		meta = (EditCondition = "bUseAutoDisableTimer"))
	float AutoDisableTime = 20.0f;

private:
	FTimerHandle AutoDisableTimerHandle;

	UFUNCTION()
	void OnRep_IsActivated();
};
