#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PS3ScreenPlayerController.generated.h"

class AControlDoor;
class ADoor;
enum class EControlDoorType : uint8;
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

	
	
//TODO 추가 사항 확인 필요: 스테이지5 전용 동작관련 변수, 함수 추가 - 김명현
#pragma region Stage5
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	
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
	
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_OperateDoor(EControlDoorType DoorType, bool bIsOpened);
	
	void OpenDoor_A();
	void OpenDoor_B();
	void OpenDoor_C();
	void OpenDoor_D();
	
	void CloseDoor_A();
	void CloseDoor_B();
	void CloseDoor_C();
	void CloseDoor_D();
	
#pragma endregion 

};
