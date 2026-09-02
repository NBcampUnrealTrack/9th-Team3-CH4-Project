#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dumbbell.generated.h"

UCLASS()
class PUZZLESTAY3_API ADumbbell : public AActor
{
	GENERATED_BODY()
	
public:	
	ADumbbell();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// 상호작용 요청 (서버 권한 컨텍스트에서 호출되어야 함)
	bool TryInteract(AActor* Requestor);
	
	// 들고 있던 걸 놓기 (마찬가지로 서버 컨텍스트에서 호출)
	void TryDrop();

	// 저울 기믹에서 사용할 무게 Getter
	UFUNCTION(BlueprintPure, Category = "Dumbbell")
	float GetWeight() const { return Weight; }
	
	// 현재 캐릭터가 들고 있는지 확인
	UFUNCTION(BlueprintPure, Category = "Dumbbell")
	bool IsHeld() const { return bIsHeld; }
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DumbbellMesh;
	
	// 덤벨을 붙일 손 소켓 (캐릭터 메쉬에 배치, 에디터에서 소켓 이름 지정)
	UPROPERTY(EditDefaultsOnly, Category = "PS3|Character|Interaction")
	FName GrabSocketName = TEXT("hand_r_socket");
	
	// 저울 계산용 무게 (기본값 1.0f)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dumbbell|Settings")
	float Weight = 1.0f;

	// 들림 상태 동기화 변수
	UPROPERTY(ReplicatedUsing = OnRep_bIsHeld)
	bool bIsHeld = false;

	UFUNCTION()
	void OnRep_bIsHeld();
	
// private:
	// 현재 나를 들고 있는 캐릭터 (서버 전용 참조, 리플리케이트 안 함)
	UPROPERTY(Replicated)
	TObjectPtr<ACharacter> HoldingCharacter;
};
