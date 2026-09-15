#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Player/Interaction/PS3InteractableInterface.h"
#include "Dumbbell.generated.h"

class UBoxComponent;
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
	
	virtual bool CanInteract_Implementation(AActor* Requestor) const override;
	virtual bool Interact_Implementation(AActor* Requestor) override;
	
	bool TryInteract(APS3PlayerCharacter* Requestor);
	bool TryDrop(APS3PlayerCharacter* Requestor);
	
	UFUNCTION(BlueprintPure, Category = "Dumbbell")
	float GetWeight() const { return Weight; }

	UFUNCTION(BlueprintPure, Category = "Dumbbell")
	bool IsHeld() const { return HoldingPlayer != nullptr; }

	APS3PlayerCharacter* GetHoldingPlayer() const { return HoldingPlayer; }
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
	UFUNCTION(BlueprintCallable, Category = "Dumbbell|UI")
	void ShowInteractionUI(bool bShow);
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DumbbellMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> TriggerBox;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dumbbell|Settings")
	FVector GrabOffset = FVector(20.0f, 20.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dumbbell|Settings")
	FRotator GrabRotationOffset = FRotator(0.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dumbbell|Settings")
	float GroundZOffset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dumbbell|Settings")
	EDumbbellType DumbbellType = EDumbbellType::Heavy;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dumbbell|Settings")
	float Weight = 200.f;
	
	UPROPERTY(ReplicatedUsing = OnRep_HoldingPlayer)
	TObjectPtr<APS3PlayerCharacter> HoldingPlayer = nullptr;
	
	UPROPERTY()
	TObjectPtr<APS3PlayerCharacter> PreviousHoldingPlayer = nullptr;
	
	UFUNCTION()
	void OnRep_HoldingPlayer();
	
private:
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
