#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "OverlapVolumeTimeDeductionComponent.generated.h"

class APS3GameModeS5;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PUZZLESTAY3_API UOverlapVolumeTimeDeductionComponent : public UBoxComponent
{
	GENERATED_BODY()
	
protected:
	UOverlapVolumeTimeDeductionComponent();
	
protected:
	virtual void BeginPlay() override;
	
	
protected:
	UFUNCTION()
	void OnCharacterOverLapped
	(
		UPrimitiveComponent* OverlappedComp, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult
	);
	
	UFUNCTION()
	void OnCharacterEndOverlap
	(
		UPrimitiveComponent* OverlappedComp, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex
	);
	
	
protected:
	UPROPERTY()
	TObjectPtr<APS3GameModeS5> PS3GameModeS5;
	
	UPROPERTY()
	TArray<TObjectPtr<AActor>> OverlappedCharacters;
	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	float DeductedTimeRange = 3.0f;


private:
	APS3GameModeS5* GetValidPS3GameModeS5();
	void OnGameStartedBind();
	void OnBindFunctionToComponent(bool bIsGameStart);
	
};
