
#include "PS3GameModeBase.h"

#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "PuzzleStay3/Component/InteractionSwitchComponent.h"

//bgimmick enum final,normal 
//if (bgimmick - normal) {APS3GameModeBase::RegisterInteractionSwitch 등록하렴}
//else(bgimmick - final) {finalRegisterInteractionSwitch 등록하렴

void APS3GameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (AllInteractionSwitchActivated())
	{
		OpenEscapeDoor();
	}
}

void APS3GameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void APS3GameModeBase::RegisterInteractionSwitch(UInteractionSwitchComponent* SwitchComp)
{
	if (!IsValid(SwitchComp)) return;
	if (InteractionSwitches.Contains(SwitchComp)) return;

	InteractionSwitches.Add(SwitchComp);

	InteractionSwitchCompoHandle =
		SwitchComp->OnSwitchActivatedChanged.AddUObject(this,&APS3GameModeBase::HandleSwitchActivatedChanged);
}

void APS3GameModeBase::UnregisterInteractionSwitch(UInteractionSwitchComponent* SwitchComp)
{
	if (!IsValid(SwitchComp)) return;

	SwitchComp->OnSwitchActivatedChanged.Remove(InteractionSwitchCompoHandle);
	InteractionSwitchCompoHandle.Reset();
	
	InteractionSwitches.Remove(SwitchComp);
}

bool APS3GameModeBase::AllInteractionSwitchActivated() const
{
	if (InteractionSwitches.IsEmpty()) { return false; }

	for (const UInteractionSwitchComponent* SwitchComp : InteractionSwitches)
	{
		if (!IsValid(SwitchComp) || !SwitchComp->IsActivated()) { return false; }
	}

	return true;
}

//델리게이트 듣는 중간함수
void APS3GameModeBase::HandleSwitchActivatedChanged(bool bActivated)
{
	OpenEscapeDoor();
}

void APS3GameModeBase::OpenEscapeDoor()
{
	
	if (AllInteractionSwitchActivated() == false) return;
	if (bEscapeDoorOpened) return;
	
	bEscapeDoorOpened = true;
	OnEscapeDoorOpened.Broadcast();
}

void APS3GameModeBase::DisableBlockingVolume(EPS3StageNumber StageNumber)
{
	OnBlockingVolumeDisabled.Broadcast(StageNumber);
}

void APS3GameModeBase::StageRestart()
{
	FString CurrentLevel = UGameplayStatics::GetCurrentLevelName(this, true);

	if (CurrentLevel.IsEmpty()) return;

	UGameplayStatics::OpenLevel(this, FName(*CurrentLevel));
}

void APS3GameModeBase::StageClear()
{
	
}
