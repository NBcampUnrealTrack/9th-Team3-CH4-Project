#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Player/Interaction/PS3InteractableInterface.h"
#include "Dumbbell.generated.h"

class APS3PlayerCharacter;

UENUM(BlueprintType)
enum class EDumbbellType : uint8
{
	Heavy   UMETA(DisplayName = "Heavy (200)"),
	Medium  UMETA(DisplayName = "Medium (180)"),
	Light   UMETA(DisplayName = "Light (150)")
};

UCLASS()
class PUZZLESTAY3_API ADumbbell : public AActor, public IPS3InteractableInterface 
{
	GENERATED_BODY()

public:
	ADumbbell();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// ★ IPS3InteractableInterface 구현
	virtual bool CanInteract_Implementation(AActor* Requestor) const override;
	virtual bool Interact_Implementation(AActor* Requestor) override;
	
	// 기존 상호작용 및 드랍
	bool TryInteract(APS3PlayerCharacter* Requestor);
	bool TryDrop(APS3PlayerCharacter* Requestor);
	
	// 저울 기믹에서 사용할 무게 Getter
	UFUNCTION(BlueprintPure, Category = "Dumbbell")
	float GetWeight() const { return Weight; }

	// 현재 캐릭터가 들고 있는지 확인
	UFUNCTION(BlueprintPure, Category = "Dumbbell")
	bool IsHeld() const { return HoldingPlayer != nullptr; }

	// 현재 들고 있는 플레이어 Getter
	APS3PlayerCharacter* GetHoldingPlayer() const { return HoldingPlayer; }
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DumbbellMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dumbbell|Settings")
	FVector GrabOffset = FVector(0.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dumbbell|Settings")
	FRotator GrabRotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dumbbell|Settings")
	EDumbbellType DumbbellType = EDumbbellType::Heavy;
	
	// 덤벨 무게
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dumbbell|Settings")
	float Weight = 200.f;
	
	// 현재 이 덤벨을 들고 있는 플레이어 (서버-클라이언트 동기화)
	UPROPERTY(ReplicatedUsing = OnRep_HoldingPlayer)
	TObjectPtr<APS3PlayerCharacter> HoldingPlayer = nullptr;

	UFUNCTION()
	void OnRep_HoldingPlayer();
};
