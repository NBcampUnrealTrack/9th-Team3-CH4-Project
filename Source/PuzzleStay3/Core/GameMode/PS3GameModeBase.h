
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "PS3GameModeBase.generated.h"


class UInteractionSwitchComponent;
class ADoor;

DECLARE_MULTICAST_DELEGATE(FOnEscapeDoorOpened);
DECLARE_MULTICAST_DELEGATE(FOnBlockingVolumeDisabled);

UCLASS()
class PUZZLESTAY3_API APS3GameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	
	//문열기 델리게이트
	FOnEscapeDoorOpened OnEscapeDoorOpened;
	
	//블록볼륨 델리게이트
	FOnBlockingVolumeDisabled OnBlockingVolumeDisabled;
	
	//b기믹스위치 스위치 개수 저장
	void RegisterInteractionSwitch(UInteractionSwitchComponent* SwitchComp);
	void UnregisterInteractionSwitch(UInteractionSwitchComponent* SwitchComp);

	//모든 b기믹스위치 활성화 여부 확인
	bool AllInteractionSwitchActivated() const;
	
	void DisableBlockingVolumes();

private:
	UFUNCTION()
	void HandleSwitchActivatedChanged(bool bActivated);
		
	void OpenEscapeDoor();
	
	UPROPERTY()
	TArray<TObjectPtr<UInteractionSwitchComponent>> InteractionSwitches;

	UPROPERTY()
	bool bEscapeDoorOpened = false;
	
	FDelegateHandle InteractionSwitchCompoHandle;
};
