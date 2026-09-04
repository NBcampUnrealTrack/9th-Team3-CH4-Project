#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "RandomCollisionTrapComponent.generated.h"

UCLASS(ClassGroup = (Gimmick), meta = (BlueprintSpawnableComponent))
class PUZZLESTAY3_API URandomCollisionTrapComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	URandomCollisionTrapComponent();
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly,
		Category = "Random Collision Trap")
	void ApplyCollisionState(bool bShouldHaveCollision);

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// GameMode의 20칸 결과 배열에서 이 발판이 사용할 위치입니다.
	// 0 = 0행 Column 0, 1 = 0행 Column 1, ..., 19 = 9행 Column 1
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Random Collision Trap",
		meta = (ClampMin = "0", ClampMax = "19"))
	int32 LayoutIndex = INDEX_NONE;
	
	UPROPERTY(ReplicatedUsing = OnRep_HasCollision,
		BlueprintReadOnly, Category = "Random Collision Trap")
	bool bHasCollision = false;

	UFUNCTION()
	void OnRep_HasCollision();

private:
	void ApplyCollisionFromStage2GameMode();
	void UpdateCollisionState();

	bool bRegisteredToStage2GameMode = false;

public:
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
