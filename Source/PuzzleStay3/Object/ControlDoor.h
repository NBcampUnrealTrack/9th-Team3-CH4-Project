#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/Enum/ControlDoorType.h"
#include "ControlDoor.generated.h"

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
	TObjectPtr<UStaticMeshComponent> DoorMesh;
	
public:
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_OnOperateDoor(EControlDoorType PressedButtonType, bool PressedType);
	
protected:
	UPROPERTY(EditAnywhere, Category = "ControlDoor|Settings")
	EControlDoorType S5_DoorType = EControlDoorType::None;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> BlockingVolumeComp;
	
	
	UPROPERTY(EditAnywhere, Category = "ControlDoor")
	TObjectPtr<UTimelineComponent> DoorTimelineComp;

	UPROPERTY(EditAnywhere, Category = "ControlDoor|Settings")
	TObjectPtr<UCurveFloat> DoorTimeLineCurve;
	
	UPROPERTY(EditAnywhere, Category = "ControlDoor|Settings")
	FVector TargetLocation = FVector(0.f, 0.f, -200.f);
	
	UPROPERTY(Replicated)
	bool bIsEscapeDoorOpen = false;
	
	UPROPERTY(Replicated)
	bool bIsDoorOpen = false;

	UFUNCTION()
	void OnTimelineUpdate(float Value);
	
	UFUNCTION()
	void OnTimelineFinished();
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMultiRPC_OnScreenPlayerSpawned();
	
	UFUNCTION(BlueprintCallable)
	void OnScreenPlayerSpawned();

		
	FVector StartLocation;
	
};
