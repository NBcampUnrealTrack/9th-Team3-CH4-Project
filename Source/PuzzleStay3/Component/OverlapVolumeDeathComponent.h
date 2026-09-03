#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "OverlapVolumeDeathComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnPlayerEnteredDeathVolume,
	APawn*,
	PlayerPawn);

UCLASS(ClassGroup = (Gimmick), meta = (BlueprintSpawnableComponent))
class PUZZLESTAY3_API UOverlapVolumeDeathComponent : public UBoxComponent
{
	GENERATED_BODY()
	
public:
	UOverlapVolumeDeathComponent();
	
	UPROPERTY(BlueprintAssignable, Category = "Life Subtract Volume")
	FOnPlayerEnteredDeathVolume OnPlayerEnteredDeathVolume;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly,
		Category = "Life Subtract Volume")
	void ResetDeathGuard(APawn* PlayerPawn);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

private:
	TSet<TWeakObjectPtr<APawn>> ProcessedPawns;
};
