#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "FakeDeathTrapComponent.generated.h"

class UStaticMeshComponent;

UCLASS(ClassGroup = (Gimmick), meta = (BlueprintSpawnableComponent))
class PUZZLESTAY3_API UFakeDeathTrapComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	UFakeDeathTrapComponent();
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Fake Death Trap")
	void ApplyLocalFakeTrapState(bool bShouldBeVisible);

	UFUNCTION(BlueprintPure, Category = "Fake Death Trap")
	int32 GetTrapId() const { return TrapId; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Fake Death Trap")
	int32 TrapId = INDEX_NONE;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Fake Death Trap")
	bool bIsVisibleForLocalPlayer = false;
	
private:
	UPROPERTY(Transient)
	TObjectPtr<UStaticMeshComponent> VisualMesh;

	void ResolveVisualMesh();
	void SetLocalVisibility(bool bShouldBeVisible);
};
