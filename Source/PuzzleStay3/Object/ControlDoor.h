#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/Enum/ControlDoorType.h"
#include "ControlDoor.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsActiveCosmetic, bool)

class UBoxComponent;
class UTimelineComponent;
enum class EControlDoorType : uint8;

UCLASS()
class PUZZLESTAY3_API AControlDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	AControlDoor();
	
	
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> DefaultSceneRoot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> ControlDoorMesh;
	
public:
	FOnIsActiveCosmetic OnIsActiveCosmetic;
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_OnOperateDoor(EControlDoorType PressedButtonType, bool bIsOpen);
	
protected:
	UPROPERTY(EditAnywhere, Category = "ControlDoor|Settings")
	EControlDoorType ControlDoorType = EControlDoorType::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ControlDoor|Settings")
	TObjectPtr<UDecalComponent> DecalComp_A;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ControlDoor|Settings")
	TObjectPtr<UDecalComponent> DecalComp_B;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ControlDoor|Settings")
	TObjectPtr<UDecalComponent> DecalComp_C;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ControlDoor|Settings")
	TObjectPtr<UDecalComponent> DecalComp_D;
	
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> BlockingVolumeComp;
	
	
	UPROPERTY(EditAnywhere, Category = "ControlDoor")
	TObjectPtr<UTimelineComponent> DoorTimelineComp;

	UPROPERTY(EditAnywhere, Category = "ControlDoor|Settings")
	TObjectPtr<UCurveFloat> DoorTimeLineCurve;
	
	UPROPERTY(EditAnywhere, Category = "ControlDoor|Settings")
	FVector TargetLocation = FVector(0.f, 0.f, -200.f);
	
	FVector StartLocation;
	
	UPROPERTY(Replicated)
	bool bIsGameStart = false;
	
	UPROPERTY(Replicated)
	bool bIsEscapeDoorOpen = false;
	
	UPROPERTY(Replicated)
	bool bIsDoorOpen = false;
	
	UPROPERTY(Replicated)
	bool bIsScreenPlayerCharacterSpawned = false;
	
	UPROPERTY(Replicated)
	bool bIsPressed = false;
	

	UFUNCTION()
	void OnTimelineUpdate(float Value);
	
	UFUNCTION()
	void OnTimelineFinished();
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMultiRPC_OnScreenPlayerSpawned();
	
	UFUNCTION(BlueprintCallable)
	void OnScreenPlayerSpawned();

	void SetVisibleDecalToDoorType();
	
	void OnGameStart(bool CurrentGameState);
		
	
private:
	void TimeLineCurveBind();
	void ErrorCheck_S5();
};
