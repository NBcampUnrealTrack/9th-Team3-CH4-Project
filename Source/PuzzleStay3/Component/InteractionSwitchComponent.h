#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionSwitchComponent.generated.h"

// 플레이어 역할 정의 Enum
UENUM(BlueprintType)
enum class EPlayerRole : uint8
{
	Any      UMETA(DisplayName = "누구나 가능"),
	PlayerA  UMETA(DisplayName = "플레이어 1"),
	PlayerB  UMETA(DisplayName = "플레이어 2")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionStateChanged, bool, bNewIsActivated);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PUZZLESTAY3_API UInteractionSwitchComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionSwitchComponent();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// 에디터 패널에서 이 스위치를 조작할 수 있는 전용 역할 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick|Role")
	EPlayerRole AllowedRole = EPlayerRole::Any;
	
	// 램프/버튼의 활성화 상태 (서버-클라이언트 동기화)
	UPROPERTY(ReplicatedUsing = OnRep_IsActivated, VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick|State")
	bool bIsActivated = false;

	// 현재 이 스위치를 점유 중인 플레이어 (1명만 할당)
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick|State")
	TObjectPtr<AActor> OccupyingPlayer = nullptr;
	
	// 외부에서 바인딩할 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Gimmick|Event")
	FOnInteractionStateChanged OnInteractionStateChanged;
	
	// PlayerCharacter F키 라인트레이스 수신 인터페이스에서 호출할 함수
	
	// 서버 권위 진입점. RPC가 아님 — 캐릭터 쪽에서 이미 검증된 서버 RPC를 통해
	// 넘어온 뒤에만 호출되어야 함. 반환값은 실제로 토글이 일어났는지 여부.
	bool TryInteract(AActor* Requestor);
	
// #pragma region Toggle
// 	
// public:
// 	// 상호작용 토글 함수
// 	UFUNCTION(BlueprintCallable, Category = "Gimmick|Logic")
// 	void Interact(AActor* Requestor);
// 	
// protected:
// 	UFUNCTION(Server, Reliable, WithValidation)
// 	void Server_Interact(AActor* Requestor);
// 	
// #pragma endregion 
	
// #pragma region Hold
// 	
// public:
// 	// 상호작용 시작 요청 (Hold 시작 시)
// 	UFUNCTION(BlueprintCallable, Category = "Gimmick|Logic")
// 	void StartInteract(AActor* Requestor);
//
// 	// 상호작용 종료/해제 요청 (Key Release 또는 범위 이탈 시)
// 	UFUNCTION(BlueprintCallable, Category = "Gimmick|Logic")
// 	void StopInteract(AActor* Requestor);
// 	
// protected:
// 	UFUNCTION(Server, Reliable, WithValidation)
// 	void Server_StartInteract(AActor* Requestor);
//
// 	UFUNCTION(Server, Reliable, WithValidation)
// 	void Server_StopInteract(AActor* Requestor);
// 	
// #pragma endregion 

protected:
	// 플레이어가 허용된 역할(Player A / Player B)이 맞는지 검증하는 함수
	bool IsRequestorAllowed(AActor* Requestor) const;
	
	UFUNCTION()
	void OnRep_IsActivated();
	
	UFUNCTION()
	void HandleOccupantDestroyed(AActor* DestroyedActor);
};
