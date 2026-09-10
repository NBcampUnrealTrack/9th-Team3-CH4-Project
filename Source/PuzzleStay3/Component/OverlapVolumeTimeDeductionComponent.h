#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Data/Enum/TimerUIType.h"
#include "OverlapVolumeTimeDeductionComponent.generated.h"

enum class EPS3TimerUIType : uint8;
class APS3GameModeS5;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PUZZLESTAY3_API UOverlapVolumeTimeDeductionComponent : public UBoxComponent
{
	GENERATED_BODY()
	
protected:
	UOverlapVolumeTimeDeductionComponent();
	
protected:
	virtual void BeginPlay() override;
	
public:
	EPS3TimerUIType TimeDeductTimerUIType = EPS3TimerUIType::None;
	
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
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMultiRPC_RemindingTimeDeduct();
	
	void ReSpawnPlayer(APlayerController* TargetPlayerController);
	
	
protected:
	UPROPERTY()
	TObjectPtr<APS3GameModeS5> CastPS3GameModeS5;
	
	UPROPERTY()
	TArray<TObjectPtr<AActor>> OverlappedCharacters;
	

	
	UPROPERTY(EditAnywhere,Category = "GameRule")
	float DeductedTimeRange = 3.0f;

	
public:
	bool bIsInteractionGimmick = true;
	
private:
	void ErrorCheck_S5();
	APS3GameModeS5* GetCastPS3GameModeS5();
	void OnBindWhenGameStarted(bool bIsGameStart);
	
};
