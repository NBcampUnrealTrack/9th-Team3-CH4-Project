#pragma once

#include "CoreMinimal.h"
#include "PS3PlayerController.h"
#include "PS3ChoiceController.generated.h"

enum class EPS3TextNotifyType : uint8;
enum class EPS3PlayerRole : uint8;


/**
 * 
 */
UCLASS()
class PUZZLESTAY3_API APS3ChoiceController : public APS3PlayerController
{
	GENERATED_BODY()
	
	APS3ChoiceController();
	
protected:
	virtual void BeginPlay() override;
	virtual void ReceivedPlayer() override;
	virtual void UpdateRotation(float DeltaTime) override;
	
	
public:
	//TODO 테스트용 BlueprintCallable 임. 나중에 삭제 할 것
	UFUNCTION(BlueprintCallable, Category = "Test")
	void OnClickedFieldTypeButton(EPS3PlayerRole SelectType);
	UFUNCTION(BlueprintCallable, Category = "Test")
	void OnClickedScreenTypeButton(EPS3PlayerRole SelectType);
	
	void ConfigureInputMapping();
	
	
public:
	UFUNCTION(Server, Reliable)
	void ServerRPC_SelectedControllerType(EPS3PlayerRole SelectedPlayerRoleType);
	
	UPROPERTY()
	TObjectPtr<class UCameraComponent> FixedCameraComponent;
	
protected:
	FTimerHandle InitTimerHandle;
	
public:
	UFUNCTION(Client, Reliable)
	void Client_ShowTextNotify(EPS3TextNotifyType NotifyType);
};

