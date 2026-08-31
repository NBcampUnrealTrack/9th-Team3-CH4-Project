#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionSwitchComponent.generated.h"

// GameModeBase에서 바인딩하는 델리게이트 
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSwitchActivatedChanged, bool);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PUZZLESTAY3_API UInteractionSwitchComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionSwitchComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 램프/버튼의 활성화 상태 (서버-클라이언트 동기화)
	UPROPERTY(ReplicatedUsing = OnRep_IsActivated, VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick")
	bool bIsActivated = false;

public:
	// GameMode가 바인딩할 델리게이트
	FOnSwitchActivatedChanged OnSwitchActivatedChanged;

	// GameMode의 AllInteractionSwitchActivated()에서 사용되는 Getter
	UFUNCTION(BlueprintCallable, Category = "Gimmick")
	bool IsActivated() const { return bIsActivated; }

	// PlayerCharacter F키 라인트레이스 수신 인터페이스에서 호출할 함수

	// 서버 권위 진입점. RPC가 아님 — 캐릭터 쪽에서 이미 검증된 서버 RPC를 통해
	// 넘어온 뒤에만 호출되어야 함. 반환값은 실제로 토글이 일어났는지 여부.
	bool TryInteract(AActor* Requestor);

protected:
	UFUNCTION()
	void OnRep_IsActivated();
};
