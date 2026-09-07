#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/Interaction/PS3InteractableInterface.h"
#include "InteractionSwitchComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSwitchActivatedChanged, bool);
DECLARE_MULTICAST_DELEGATE(FOnInteractionSuccessed);

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
	// GameMode가 바인딩할 델리게이트
	FOnSwitchActivatedChanged OnSwitchActivatedChanged;
	FOnInteractionSuccessed OnInteractionSuccessed;
	
	// GameMode의 AllInteractionSwitchActivated()에서 사용되는 Getter
	UFUNCTION(BlueprintCallable, Category = "Gimmick")
	bool IsActivated() const { return bIsActivated; }

	// PlayerCharacter F키 라인트레이스 수신 인터페이스에서 호출할 함수
	bool TryInteract(AActor* Requestor);
	void SetRegisterToGameMode(bool bRegister) { bRegisterToGameMode = bRegister; }
	
	// 타이머 시작 함수
	void StartDisableTimer();
	
	// 타이머 만료/리셋 시 스위치 꺼짐 처리
	void ResetSwitch();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick")
	bool bIsEscapeDoor = true;
	
protected:
	// 램프/버튼의 활성화 상태
	UPROPERTY(ReplicatedUsing = OnRep_IsActivated, VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick")
	bool bIsActivated = false;

	// GameMode의 글로벌 퍼즐 스위치 목록에 등록할지 여부
	// 일반 퍼즐 스위치는 true, 저울 버튼 등 독립 기믹 스위치는 false로 설정
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick")
	bool bRegisterToGameMode = true;
	
	// 비긴플레이 단계에서 타이머를 가동하여 시작할지 체크
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|Timer")
	bool bStartTimerOnBeginPlay = false;
	
	// 상호작용(TryInteract) 시에도 타이머를 사용할지 체크
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|Timer")
	bool bUseAutoDisableTimer = false;
	
	// 타이머 시간 (초 단위)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|Timer", meta = (EditCondition = "bUseAutoDisableTimer"))
	float AutoDisableTime = 20.0f;
	
private:
	FTimerHandle AutoDisableTimerHandle;

	UFUNCTION()
	void OnRep_IsActivated();
};
