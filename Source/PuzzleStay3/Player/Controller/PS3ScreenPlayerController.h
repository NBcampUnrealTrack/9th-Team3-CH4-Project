#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/ControlDoorType.h"
#include "GameFramework/PlayerController.h"
#include "PS3ScreenPlayerController.generated.h"

struct FInputActionInstance;
enum class EControlDoorType : uint8;
class AControlDoor;
class ADoor;
class UInputMappingContext;
class UInputAction;

UCLASS()
class PUZZLESTAY3_API APS3ScreenPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APS3ScreenPlayerController();

	UFUNCTION(BlueprintPure, Category = "PS3|Screen Player Controller")
	bool IsScreenPlayer() const;
	

	
	
	virtual void ReceivedPlayer() override;
	
private:
	void ConfigureLocalInputMode();
	bool bLocalInputModeConfigured = false;

	
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	EControlDoorType CurrentOpenedDoorType = EControlDoorType::None;
	
	void TryOpenDoor(EControlDoorType DoorType);
	void TryCloseDoor(EControlDoorType DoorType);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS3|Screen Controller|Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS3|Screen Player Controller|Input")
	TObjectPtr<UInputAction> Button_A;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS3|Screen Player Controller|Input")
	TObjectPtr<UInputAction> Button_B;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS3|Screen Player Controller|Input")
	TObjectPtr<UInputAction> Button_C;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS3|Screen Player Controller|Input")
	TObjectPtr<UInputAction> Button_D;
	
	UPROPERTY()
	TArray<AControlDoor*> ControlDoorArray;
	
	FTimerHandle PS3CameraTimerHandle;
	
	bool bIsPressHolding = false;
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_OperateDoor(EControlDoorType DoorType, bool bIsOpen);
	
	EControlDoorType GetDoorTypeFromAction(const UInputAction* Action) const;
	
	void OpenDoor(const FInputActionInstance& Instance);
	void CloseDoor(const FInputActionInstance& Instance);
	
	void OpenDoor_A();
	void OpenDoor_B();
	void OpenDoor_C();
	void OpenDoor_D();
	
	void CloseDoor_A();
	void CloseDoor_B();
	void CloseDoor_C();
	void CloseDoor_D();
	
	void SetCameraView();
	
};
