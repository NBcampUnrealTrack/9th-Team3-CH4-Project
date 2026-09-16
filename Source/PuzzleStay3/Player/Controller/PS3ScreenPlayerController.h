#pragma once

#include "CoreMinimal.h"
#include "PS3PlayerControllerBase.h"
#include "Data/Enum/ControlDoorType.h"
#include "PS3ScreenPlayerController.generated.h"

enum class EControlDoorType : uint8;
struct FInputActionInstance;
class AControlDoor;
class ADoor;
class UInputMappingContext;
class UInputAction;


UENUM()
enum class EDeBugType : uint8
{
	OriginController,
	TestController
	
};

UCLASS()
class PUZZLESTAY3_API APS3ScreenPlayerController : public APS3PlayerControllerBase
{
	GENERATED_BODY()

public:
	APS3ScreenPlayerController();

	
private:
	void ConfigureLocalInputMode();
	bool bLocalInputModeConfigured = false;
	
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void ReceivedPlayer() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void ConfigureViewModelBindings(UPS3ViewModel* InViewModel) override;
	
	
	EControlDoorType CurrentOpenedDoorType = EControlDoorType::None;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ScreenController|Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ScreenController|Input")
	TObjectPtr<UInputAction> Button_A;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ScreenController|Input")
	TObjectPtr<UInputAction> Button_B;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ScreenController|Input")
	TObjectPtr<UInputAction> Button_C;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ScreenController|Input")
	TObjectPtr<UInputAction> Button_D;
	
	UPROPERTY()
	TArray<AControlDoor*> ControlDoorArray;
	
	FTimerHandle PS3CameraTimerHandle;
	
	bool bIsPressHolding = false;
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_OperateDoor(EControlDoorType DoorType, bool bIsOpen);
	
	EControlDoorType GetDoorTypeFromAction(const UInputAction* Action) const;
	
	void ContainDoorArray();
	
	void OpenDoor(const FInputActionInstance& Instance);
	void CloseDoor(const FInputActionInstance& Instance);
	
	void SetCameraView();
	
	void ShowToCeiling();
	void HideToCeiling();
	
	void OnScreenPlayerUI_Show() const;
	
private:
	UPROPERTY(EditAnywhere, Category = "ScreenController|TestControllerSetting")
	EDeBugType DeBugType = EDeBugType::OriginController;
	
	
	void TestToggledDoor(const FInputActionInstance& Instance);
	
};
