#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RandomCollisionTrapComponent.generated.h"

class ARandomCollisionTrap;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PUZZLESTAY3_API URandomCollisionTrapComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URandomCollisionTrapComponent();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly,
		Category = "Random Collision Trap")
	void InitializePlatforms(int32 InSeed);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly,
		Category = "Random Collision Trap")
	void ResetPlatforms(int32 InSeed);
	
protected:
	//테스트용----------
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Random Collision Trap")
	int32 InitialSeed = 12345;
	//테스트용----------
	
	UPROPERTY(EditAnywhere, Category = "Random Collision Trap")
	int32 RowCount = 10;

	UPROPERTY(EditAnywhere, Category = "Random Collision Trap")
	int32 ColumnCount = 2;

	UPROPERTY(EditAnywhere, Category = "Random Collision Trap")
	TSubclassOf<ARandomCollisionTrap> TrapClass;

	UPROPERTY(EditAnywhere, Category = "Random Collision Trap")
	FVector StartLocalLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Random Collision Trap")
	FVector RowLocalOffset = FVector(300.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, Category = "Random Collision Trap")
	FVector ColumnLocalOffset = FVector(0.0f, 250.0f, 0.0f);

private:
	UPROPERTY()
	TArray<TObjectPtr<ARandomCollisionTrap>> Platforms;

	void SpawnPlatforms();
	void ApplyRandomCollisionLayout(int32 InSeed);
		
};
