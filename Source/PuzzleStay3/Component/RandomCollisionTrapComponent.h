#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "RandomCollisionTrapComponent.generated.h"

class UStaticMesh;
class UStaticMeshComponent;

UCLASS(ClassGroup = (Gimmick), meta = (BlueprintSpawnableComponent))
class PUZZLESTAY3_API URandomCollisionTrapComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	URandomCollisionTrapComponent();
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly,
		Category = "Random Collision Trap")
	void ApplyCollisionLayout(const TArray<bool>& InCollisionLayout);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Random Collision Trap")
	int32 RowCount = 10;

	UPROPERTY(EditAnywhere, Category = "Random Collision Trap")
	int32 ColumnCount = 2;

	UPROPERTY(EditAnywhere, Category = "Random Collision Trap")
	TObjectPtr<UStaticMesh> PlatformMeshAsset;

	UPROPERTY(EditAnywhere, Category = "Random Collision Trap")
	FVector StartLocalLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Random Collision Trap")
	FVector RowLocalOffset = FVector(300.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, Category = "Random Collision Trap")
	FVector ColumnLocalOffset = FVector(0.0f, 250.0f, 0.0f);

	UPROPERTY(EditAnywhere, Category = "Random Collision Trap")
	FVector PlatformCollisionExtent = FVector(200.0f, 200.0f, 30.0f);
	
	UPROPERTY(ReplicatedUsing = OnRep_CollisionLayout,
		BlueprintReadOnly, Category = "Random Collision Trap")
	TArray<uint8> CollisionLayout;

	UFUNCTION()
	void OnRep_CollisionLayout();

	void SpawnPlatforms();
	void ApplyCollisionLayoutToPlatforms();

	UPROPERTY(Transient)
	TArray<TObjectPtr<UStaticMeshComponent>> PlatformMeshes;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UBoxComponent>> PlatformCollisions;

public:
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
