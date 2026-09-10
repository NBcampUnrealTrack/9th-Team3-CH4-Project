#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/Interaction/PS3InteractableInterface.h"
#include "InteractionSwitchComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSwitchActivatedChanged, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInteractionSuccessed, bool);
DECLARE_MULTICAST_DELEGATE(FOnCosmeticInteractionSuccessed);

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

	// GameMode의 AllInteractionSwitchActivated()에서 사용되는 Getter
	UFUNCTION(BlueprintCallable, Category = "Gimmick")
	bool IsActivated() const { return bIsActivated; }

	// PlayerCharacter F키 라인트레이스 수신 인터페이스에서 호출할 함수
	bool TryInteract(AActor* Requestor);
	void SetRegisterToGameMode(bool bRegister) { bRegisterToGameMode = bRegister; }

	// 타이머 시작 /리셋 시 사용 함수
	void StartDisableTimer();
	void ResetSwitch();

	// 퍼즐 완결 등 상호작용을 영구 차단할 때 사용
	UFUNCTION(BlueprintCallable, Category = "Gimmick")
	void SetLocked(bool bNewLocked) { bIsLocked = bNewLocked; }

	// 현재 스위치를 켠 주체 Getter
	AActor* GetInteractingActor() const { return InteractingActor; }

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "Gimmick")
	bool bIsEscapeDoor = true;
	
	UPROPERTY(Replicated)
	bool bIsInteractedGimmick = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick|Settings")
	int32 SwitchID = 1;

protected:
	// true: 스위치를 켜면 플레이어가 상호작용 중(IsInteracting) 상태가 됨 (다른 상호작용 불가)
	// false: 스위치를 켜도 플레이어 상태를 바꾸지 않음 (혼자서 4개 스위치 연속 조작 가능)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick")
	bool bToggleInteractionState = true; 
	
	// 램프/버튼의 활성화 상태
	UPROPERTY(ReplicatedUsing = OnRep_IsActivated, VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick")
	bool bIsActivated = false;

	// true일 경우 CanInteract가 항상 false를 반환
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick")
	bool bIsLocked = false;
	
	// 현재 이 스위치를 선점/조작한 플레이어 (서버 동기화)
	UPROPERTY(Replicated, Transient, VisibleAnywhere, Category = "Gimmick")
	TObjectPtr<AActor> InteractingActor = nullptr;

	// GameMode의 글로벌 퍼즐 스위치 목록에 등록할지 여부 일반 퍼즐 스위치는 true, 저울 버튼 등 독립 기믹 스위치는 false로 설정
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick")
	bool bRegisterToGameMode = true;

	// 상호작용(TryInteract) 시에도 타이머를 사용할지 체크
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|Timer")
	bool bUseAutoDisableTimer = false;

	// 타이머 시간 (초 단위)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|Timer",
		meta = (EditCondition = "bUseAutoDisableTimer"))
	float AutoDisableTime = 20.0f;

private:
	FTimerHandle AutoDisableTimerHandle;

	UFUNCTION()
	void OnRep_IsActivated();
};
