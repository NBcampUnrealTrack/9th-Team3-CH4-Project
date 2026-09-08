#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "FakeDeathTrapComponent.generated.h"

class UStaticMeshComponent;
class APawn;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnFakeDeathTrapOverlapped,
	APawn*,
	PlayerPawn);


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
	
	UPROPERTY(BlueprintAssignable, Category = "Fake Death Trap|Events")
	FOnFakeDeathTrapOverlapped OnFakeDeathTrapOverlapped;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Fake Death Trap")
	int32 TrapId = INDEX_NONE;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Fake Death Trap")
	bool bIsVisibleForLocalPlayer = false;
	
	UFUNCTION()
	void HandleBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	
private:
	UPROPERTY(Transient)
	TObjectPtr<UStaticMeshComponent> VisualMesh;

	void ResolveVisualMesh();
	void SetLocalVisibility(bool bShouldBeVisible);
	
	bool bRegisteredToGameModeS3 = false;
};
