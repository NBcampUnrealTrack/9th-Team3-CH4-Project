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

	// GameMode의 글로벌 퍼즐 스위치 목록에 등록할지 여부
	// 일반 퍼즐 스위치는 true, 저울 버튼 등 독립 기믹 스위치는 false로 설정
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick")
	bool bRegisterToGameMode = true;

public:
	// GameMode가 바인딩할 델리게이트
	FOnSwitchActivatedChanged OnSwitchActivatedChanged;

	// GameMode의 AllInteractionSwitchActivated()에서 사용되는 Getter
	UFUNCTION(BlueprintCallable, Category = "Gimmick")
	bool IsActivated() const { return bIsActivated; }

	// PlayerCharacter F키 라인트레이스 수신 인터페이스에서 호출할 함수
	bool TryInteract(AActor* Requestor);
	
	void SetRegisterToGameMode(bool bRegister) { bRegisterToGameMode = bRegister; }

	// 수동으로 스위치 상태를 Off(false)로 리셋하는 함수
	void ResetSwitch();
	
	bool bIsEscapeDoor = true;
	
protected:
	UFUNCTION()
	void OnRep_IsActivated();
};
