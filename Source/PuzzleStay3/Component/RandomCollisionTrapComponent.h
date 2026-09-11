#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "RandomCollisionTrapComponent.generated.h"

class APawn;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnFakePlatformOverlapped,
	APawn*,
	PlayerPawn);


UCLASS(ClassGroup = (Gimmick), meta = (BlueprintSpawnableComponent))
class PUZZLESTAY3_API URandomCollisionTrapComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	URandomCollisionTrapComponent();
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly,
		Category = "Random Collision Trap")
	void ApplyCollisionState(bool bShouldHaveCollision);

	UPROPERTY(BlueprintAssignable, Category = "Random Collision Trap|Events")
	FOnFakePlatformOverlapped OnFakePlatformOverlapped;
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFakePlatformOverlapped(APawn* PlayerPawn);
	
protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Random Collision Trap",
		meta = (ClampMin = "0", ClampMax = "19"))
	int32 LayoutIndex = INDEX_NONE;
	
	UPROPERTY(ReplicatedUsing = OnRep_HasCollision,
		BlueprintReadOnly, Category = "Random Collision Trap")
	bool bHasCollision = false;

	UFUNCTION()
	void OnRep_HasCollision();
	
	UFUNCTION()
	void HandleBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

private:
	void ApplyCollisionFromStage2GameMode();
	void UpdateCollisionState();

	bool bRegisteredToStage2GameMode = false;

public:
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
