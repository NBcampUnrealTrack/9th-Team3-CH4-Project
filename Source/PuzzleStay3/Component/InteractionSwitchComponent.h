#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionSwitchComponent.generated.h"

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
	// 램프/버튼의 활성화 상태 (서버-클라이언트 동기화)
	UPROPERTY(ReplicatedUsing = OnRep_IsActivated, VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick|State")
	bool bIsActivated;

	// 외부(발광 연출, Light FX, SFX 등)에서 바인딩할 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Gimmick|Event")
	FOnInteractionStateChanged OnInteractionStateChanged;
	
	// 준현 님의 PlayerCharacter F키 라인트레이스 수신 인터페이스에서 호출할 함수[cite: 1]
	UFUNCTION(BlueprintCallable, Category = "Gimmick|Logic")
	void Interact();

	// 서버 권한으로 상태 변경 처리
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Interact();

protected:
	UFUNCTION()
	void OnRep_IsActivated();
};
