#include "PS3ViewModel.h"

#include "Core/GameState/PS3GameStateS5.h"
#include "Data/DataAsset/S5_GameRuleDataAsset.h"
#include "Data/Delegates/UIDelegatesSubsystem.h"
#include "../HUD/PlayerHUD.h"

namespace
{
	constexpr float DefaultTimerMaxTime = 7.0f;
	constexpr float InteractionSwitchTimerMaxTime = 20.0f;
}

void UPS3ViewModel::SetPlayerHUD(APlayerHUD* InPlayerHUD)
{
	PlayerHUD = InPlayerHUD;
	BindRoleSelectionUIDelegate();
	BindGameplayUIDelegates();
}

void UPS3ViewModel::BeginDestroy()
{
	UnbindRoleSelectionUIDelegate();
	UnbindGameplayUIDelegates();

	Super::BeginDestroy();
}

void UPS3ViewModel::BindRoleSelectionUIDelegate()
{
	UUIDelegatesSubsystem* UIDelegatesSubsystem =
		UUIDelegatesSubsystem::GetUIDelegateManager(this);
	if (!IsValid(UIDelegatesSubsystem))
	{
		return;
	}

	UIDelegatesSubsystem->OnRoleSelection_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnRoleSelection_UI.AddUObject(
		this,
		&ThisClass::HandleRoleSelection_UI);
}

void UPS3ViewModel::UnbindRoleSelectionUIDelegate()
{
	UUIDelegatesSubsystem* UIDelegatesSubsystem =
		UUIDelegatesSubsystem::GetUIDelegateManager(this);
	if (!IsValid(UIDelegatesSubsystem))
	{
		return;
	}

	UIDelegatesSubsystem->OnRoleSelection_UI.RemoveAll(this);
}

void UPS3ViewModel::HandleRoleSelection_UI(bool bVisible)
{
	RequestSetStage5RoleSelectVisible(bVisible);
}

void UPS3ViewModel::BindGameplayUIDelegates()
{
	UUIDelegatesSubsystem* UIDelegatesSubsystem =
		UUIDelegatesSubsystem::GetUIDelegateManager(this);
	if (!IsValid(UIDelegatesSubsystem))
	{
		return;
	}

	UIDelegatesSubsystem->OnStageType_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnStageType_UI.AddUObject(
		this,
		&ThisClass::HandleStageType_UI);

	UIDelegatesSubsystem->OnScreenPlayer_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnScreenPlayer_UI.AddUObject(
		this,
		&ThisClass::HandleScreenPlayer_UI);

	UIDelegatesSubsystem->OnFieldPlayer_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnFieldPlayer_UI.AddUObject(
		this,
		&ThisClass::HandleFieldPlayer_UI);

	UIDelegatesSubsystem->OnVoiceChatIcon_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnVoiceChatIcon_UI.AddUObject(
		this,
		&ThisClass::HandleVoiceChatIcon_UI);

	UIDelegatesSubsystem->OnOptionPopup_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnOptionPopup_UI.AddUObject(
		this,
		&ThisClass::HandleOptionPopup_UI);

	UIDelegatesSubsystem->OnIsGameOver_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnIsGameOver_UI.AddUObject(
		this,
		&ThisClass::HandleIsGameOver_UI);

	UIDelegatesSubsystem->OnTutorial_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnTutorial_UI.AddUObject(
		this,
		&ThisClass::HandleTutorial_UI);

	UIDelegatesSubsystem->OnTitle_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnTitle_UI.AddUObject(
		this,
		&ThisClass::HandleTitle_UI);

	UIDelegatesSubsystem->OnTimerNotifyVisible_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnTimerNotifyVisible_UI.AddUObject(
		this,
		&ThisClass::HandleTimerNotifyVisible_UI);

	UIDelegatesSubsystem->OnLifeCount_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnLifeCount_UI.AddUObject(
		this,
		&ThisClass::HandleLifeCount_UI);

	UIDelegatesSubsystem->OnButtonEnabled_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnButtonEnabled_UI.AddUObject(
		this,
		&ThisClass::HandleButtonEnabled_UI);

	UIDelegatesSubsystem->OnGameTimer_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnGameTimer_UI.AddUObject(
		this,
		&ThisClass::HandleGameTimer_UI);

	UIDelegatesSubsystem->OnTimeDeduct_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnTimeDeduct_UI.AddUObject(
		this,
		&ThisClass::HandleTimeDeduct_UI);

	UIDelegatesSubsystem->OnTimerReset_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnTimerReset_UI.AddUObject(
		this,
		&ThisClass::HandleTimerReset_UI);

	UIDelegatesSubsystem->OnInteractionNotifyAddRequested_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnInteractionNotifyAddRequested_UI.AddUObject(
		this,
		&ThisClass::HandleInteractionNotifyAddRequested_UI);

	UIDelegatesSubsystem->OnInteractionNotifyRemoveRequested_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnInteractionNotifyRemoveRequested_UI.AddUObject(
		this,
		&ThisClass::HandleInteractionNotifyRemoveRequested_UI);

	UIDelegatesSubsystem->OnInteractionNotifyResetRequested_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnInteractionNotifyResetRequested_UI.AddUObject(
		this,
		&ThisClass::HandleInteractionNotifyResetRequested_UI);

	UIDelegatesSubsystem->OnTextNotifyVisible_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnTextNotifyVisible_UI.AddUObject(
		this,
		&ThisClass::HandleTextNotifyVisible_UI);

	UIDelegatesSubsystem->OnTextNotify_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnTextNotify_UI.AddUObject(
		this,
		&ThisClass::HandleTextNotify_UI);
}

void UPS3ViewModel::UnbindGameplayUIDelegates()
{
	UUIDelegatesSubsystem* UIDelegatesSubsystem =
		UUIDelegatesSubsystem::GetUIDelegateManager(this);
	if (!IsValid(UIDelegatesSubsystem))
	{
		return;
	}

	UIDelegatesSubsystem->OnStageType_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnScreenPlayer_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnFieldPlayer_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnVoiceChatIcon_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnOptionPopup_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnIsGameOver_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnTutorial_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnTitle_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnTimerNotifyVisible_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnLifeCount_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnButtonEnabled_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnGameTimer_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnTimeDeduct_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnTimerReset_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnInteractionNotifyAddRequested_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnInteractionNotifyRemoveRequested_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnInteractionNotifyResetRequested_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnTextNotifyVisible_UI.RemoveAll(this);
	UIDelegatesSubsystem->OnTextNotify_UI.RemoveAll(this);
}

void UPS3ViewModel::HandleStageType_UI(EPS3StageType StageType)
{
	CurrentStageType = StageType;
	ApplyStageUI();
}

void UPS3ViewModel::RefreshStageUI()
{
	ApplyStageUI();
}

void UPS3ViewModel::HandleScreenPlayer_UI(bool bVisible)
{
	if (CurrentStageType == EPS3StageType::Stage5 && bVisible)
	{
		RequestSetDoorOpenButtonVisible(true);
		RequestSetInteractionNotifyVisible(false);
	}
}

void UPS3ViewModel::HandleFieldPlayer_UI(bool bVisible)
{
	if (CurrentStageType == EPS3StageType::Stage5 && bVisible)
	{
		RequestSetDoorOpenButtonVisible(false);
		RequestSetInteractionNotifyVisible(true);
	}
}

void UPS3ViewModel::HandleVoiceChatIcon_UI(bool bVisible)
{
	bVoiceChatIconEnabled = bVisible;

	if (!bVoiceChatIconEnabled)
	{
		RequestSetVoiceChatIconVisible(false);
	}
}

void UPS3ViewModel::HandleOptionPopup_UI(bool bVisible)
{
	RequestSetOptionPopupVisible(bVisible);
}

void UPS3ViewModel::HandleIsGameOver_UI(bool bVisible)
{
	RequestSetGameOverVisible(bVisible);
}

void UPS3ViewModel::HandleTutorial_UI(bool bVisible)
{
	RequestSetTutorialNotifyVisible(bVisible);
}

void UPS3ViewModel::HandleTitle_UI(bool bVisible)
{
	RequestSetTitleVisible(bVisible);
}

void UPS3ViewModel::HandleTimerNotifyVisible_UI(bool bVisible)
{
	RequestSetTimerNotifyVisible(bVisible);
}

void UPS3ViewModel::HandleLifeCount_UI(int32 InCurrentLifeCount, int32 InMaxLifeCount)
{
	RequestUpdateLifeCount(InCurrentLifeCount, InMaxLifeCount);
}

void UPS3ViewModel::HandleButtonEnabled_UI(EControlDoorType DoorType, bool)
{
	switch (DoorType)
	{
	case EControlDoorType::None:
		SetIsDoor1Unlocked(true);
		SetIsDoor2Unlocked(true);
		SetIsDoor3Unlocked(true);
		SetIsDoor4Unlocked(true);
		RequestSetDoorPressedFilters(false, false, false, false);
		break;
	case EControlDoorType::Door_A:
		SetIsDoor1Unlocked(false);
		SetIsDoor2Unlocked(true);
		SetIsDoor3Unlocked(true);
		SetIsDoor4Unlocked(true);
		RequestSetDoorPressedFilters(true, false, false, false);
		break;
	case EControlDoorType::Door_B:
		SetIsDoor1Unlocked(true);
		SetIsDoor2Unlocked(false);
		SetIsDoor3Unlocked(true);
		SetIsDoor4Unlocked(true);
		RequestSetDoorPressedFilters(false, true, false, false);
		break;
	case EControlDoorType::Door_C:
		SetIsDoor1Unlocked(true);
		SetIsDoor2Unlocked(true);
		SetIsDoor3Unlocked(false);
		SetIsDoor4Unlocked(true);
		RequestSetDoorPressedFilters(false, false, true, false);
		break;
	case EControlDoorType::Door_D:
		SetIsDoor1Unlocked(true);
		SetIsDoor2Unlocked(true);
		SetIsDoor3Unlocked(true);
		SetIsDoor4Unlocked(false);
		RequestSetDoorPressedFilters(false, false, false, true);
		break;
	default:
		break;
	}
}

void UPS3ViewModel::HandleGameTimer_UI(EPS3TimerUIType TimerUIType, float CurrentTime)
{
	if (TimerUIType == EPS3TimerUIType::None)
	{
		return;
	}

	const FName TimerId(*StaticEnum<EPS3TimerUIType>()->GetNameStringByValue(static_cast<int64>(TimerUIType)));
	RequestTimerNotify(TimerId, ResolveTimerMaxTime(TimerUIType), CurrentTime);
}

void UPS3ViewModel::HandleTimeDeduct_UI(EPS3TimerUIType TimerUIType, float ReduceTime)
{
	if (TimerUIType == EPS3TimerUIType::None)
	{
		return;
	}

	const FName TimerId(*StaticEnum<EPS3TimerUIType>()->GetNameStringByValue(static_cast<int64>(TimerUIType)));
	RequestReduceTimerNotify(TimerId, ReduceTime);
}

void UPS3ViewModel::HandleTimerReset_UI()
{
	RequestResetTimerNotify();
}

void UPS3ViewModel::HandleInteractionNotifyAddRequested_UI(EPS3InteractionNotifyType NotifyType)
{
	RequestShowInteractionNotify(NotifyType);
}

void UPS3ViewModel::HandleInteractionNotifyRemoveRequested_UI(EPS3InteractionNotifyType NotifyType)
{
	RequestHideInteractionNotify(NotifyType);
}

void UPS3ViewModel::HandleInteractionNotifyResetRequested_UI()
{
	RequestHideAllInteractionNotifies();
}

void UPS3ViewModel::HandleTextNotifyVisible_UI(bool bVisible)
{
	bTextNotifyEnabled = bVisible;

	if (!bTextNotifyEnabled)
	{
		RequestSetTextNotifyVisible(false);
	}
}

void UPS3ViewModel::HandleTextNotify_UI(EPS3TextNotifyType NotifyType)
{
	if (!bTextNotifyEnabled)
	{
		return;
	}

	RequestTextNotify(NotifyType);
}

float UPS3ViewModel::ResolveTimerMaxTime(EPS3TimerUIType TimerUIType) const
{
	if (TimerUIType == EPS3TimerUIType::GimmickB_1 ||
		TimerUIType == EPS3TimerUIType::GimmickB_2 ||
		TimerUIType == EPS3TimerUIType::GimmickB_3 ||
		TimerUIType == EPS3TimerUIType::GimmickB_4)
	{
		return InteractionSwitchTimerMaxTime;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return DefaultTimerMaxTime;
	}

	const APS3GameStateS5* GameState = World->GetGameState<APS3GameStateS5>();
	if (!IsValid(GameState) || !IsValid(GameState->S5_GameRuleDataAsset))
	{
		return DefaultTimerMaxTime;
	}

	if (TimerUIType == EPS3TimerUIType::GameStartTimer)
	{
		return GameState->S5_GameRuleDataAsset->MaxGameLimitTime;
	}

	return DefaultTimerMaxTime;
}

void UPS3ViewModel::ApplyStageUI()
{
	switch (CurrentStageType)
	{
	case EPS3StageType::Stage1:
		RequestSetLifeCountVisible(false);
		bVoiceChatIconEnabled = true;
		RequestSetInteractionNotifyVisible(true);
		bTextNotifyEnabled = true;
		RequestSetTimerNotifyVisible(true);
		RequestSetDoorOpenButtonVisible(false);
		break;
	case EPS3StageType::Stage2:
		RequestSetLifeCountVisible(true);
		bVoiceChatIconEnabled = true;
		RequestSetInteractionNotifyVisible(true);
		bTextNotifyEnabled = true;
		RequestSetTimerNotifyVisible(false);
		RequestSetDoorOpenButtonVisible(false);
		break;
	case EPS3StageType::Stage3:
		bVoiceChatIconEnabled = false;
		RequestSetVoiceChatIconVisible(false);
		RequestSetInteractionNotifyVisible(true);
		bTextNotifyEnabled = true;
		RequestSetTimerNotifyVisible(false);
		RequestSetDoorOpenButtonVisible(false);
		break;
	case EPS3StageType::Stage4:
		RequestSetLifeCountVisible(false);
		bVoiceChatIconEnabled = true;
		RequestSetInteractionNotifyVisible(true);
		bTextNotifyEnabled = true;
		RequestSetTimerNotifyVisible(false);
		RequestSetDoorOpenButtonVisible(false);
		break;
	case EPS3StageType::Stage5:
		RequestSetLifeCountVisible(false);
		bVoiceChatIconEnabled = true;
		RequestSetInteractionNotifyVisible(true);
		bTextNotifyEnabled = true;
		RequestSetTimerNotifyVisible(true);
		RequestSetDoorOpenButtonVisible(false);
		break;
	default:
		break;
	}
}

void UPS3ViewModel::RequestTextNotify(EPS3TextNotifyType NotifyType)
{
	if (PlayerHUD)
	{
		PlayerHUD->ShowTextNotify(NotifyType);
	}
}

void UPS3ViewModel::RequestSetTextNotifyVisible(bool bVisible)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetTextNotifyVisible(bVisible);
	}
}

void UPS3ViewModel::RequestSetLifeCountVisible(bool bVisible)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetLifeCountVisible(bVisible);
	}
}

void UPS3ViewModel::RequestUpdateLifeCount(int32 InCurrentLifeCount, int32 InMaxLifeCount)
{
	if (PlayerHUD)
	{
		PlayerHUD->UpdateLifeCount(InCurrentLifeCount, InMaxLifeCount);
	}
}

void UPS3ViewModel::RequestSetInteractionNotifyVisible(bool bVisible)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetInteractionNotifyVisible(bVisible);
	}
}

void UPS3ViewModel::RequestShowInteractionNotify(EPS3InteractionNotifyType NotifyType)
{
	if (PlayerHUD)
	{
		PlayerHUD->ShowInteractionNotify(NotifyType);
	}
}

void UPS3ViewModel::RequestHideInteractionNotify(EPS3InteractionNotifyType NotifyType)
{
	if (PlayerHUD)
	{
		PlayerHUD->HideInteractionNotify(NotifyType);
	}
}

void UPS3ViewModel::RequestHideAllInteractionNotifies()
{
	if (PlayerHUD)
	{
		PlayerHUD->HideAllInteractionNotifies();
	}
}

void UPS3ViewModel::RequestSetTimerNotifyVisible(bool bVisible)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetTimerNotifyVisible(bVisible);
	}
}

void UPS3ViewModel::RequestTimerNotify(FName InTimerId, float InMaxTime, float InCurrentTime)
{
	if (PlayerHUD)
	{
		PlayerHUD->UpdateTimerNotify(InTimerId, InMaxTime, InCurrentTime);
	}
}

void UPS3ViewModel::RequestReduceTimerNotify(FName InTimerId, float InReduceTime)
{
	if (PlayerHUD)
	{
		PlayerHUD->ReduceTimerNotify(InTimerId, InReduceTime);
	}
}

void UPS3ViewModel::RequestResetTimerNotify()
{
	if (PlayerHUD)
	{
		PlayerHUD->ResetTimerNotify();
	}
}

void UPS3ViewModel::RequestSetTutorialNotifyVisible(bool bVisible)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetTutorialNotifyVisible(bVisible);
	}
}

void UPS3ViewModel::RequestSetDoorOpenButtonVisible(bool bVisible)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetDoorOpenButtonVisible(bVisible);
	}
}

void UPS3ViewModel::RequestSetDoorPressedFilters(
	bool bDoor1Pressed,
	bool bDoor2Pressed,
	bool bDoor3Pressed,
	bool bDoor4Pressed
)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetDoorPressedFilters(bDoor1Pressed, bDoor2Pressed, bDoor3Pressed, bDoor4Pressed);
	}
}

int32 UPS3ViewModel::GetCurrentLifeCount() const
{
	return CurrentLifeCount;
}

void UPS3ViewModel::SetCurrentLifeCount(int32 InCurrentLifeCount)
{
	UE_MVVM_SET_PROPERTY_VALUE(CurrentLifeCount, InCurrentLifeCount);
}

int32 UPS3ViewModel::GetMaxLifeCount() const
{
	return MaxLifeCount;
}

void UPS3ViewModel::SetMaxLifeCount(int32 InMaxLifeCount)
{
	UE_MVVM_SET_PROPERTY_VALUE(MaxLifeCount, InMaxLifeCount);
}

TArray<FText> UPS3ViewModel::GetInteractionKeyNames() const
{
	return InteractionKeyNames;
}

void UPS3ViewModel::SetInteractionKeyNames(const TArray<FText>& InInteractionKeyNames)
{
	UE_MVVM_SET_PROPERTY_VALUE(InteractionKeyNames, InInteractionKeyNames);
}

bool UPS3ViewModel::GetIsInteractionNotifyVisible() const
{
	return bIsInteractionNotifyVisible;
}

void UPS3ViewModel::SetIsInteractionNotifyVisible(bool bInIsInteractionNotifyVisible)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsInteractionNotifyVisible, bInIsInteractionNotifyVisible);
}

bool UPS3ViewModel::GetIsDoor1Unlocked() const
{
	return bIsDoor1Unlocked;
}

void UPS3ViewModel::SetIsDoor1Unlocked(bool bInIsDoor1Unlocked)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsDoor1Unlocked, bInIsDoor1Unlocked);
}

bool UPS3ViewModel::GetIsDoor2Unlocked() const
{
	return bIsDoor2Unlocked;
}

void UPS3ViewModel::SetIsDoor2Unlocked(bool bInIsDoor2Unlocked)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsDoor2Unlocked, bInIsDoor2Unlocked);
}

bool UPS3ViewModel::GetIsDoor3Unlocked() const
{
	return bIsDoor3Unlocked;
}

void UPS3ViewModel::SetIsDoor3Unlocked(bool bInIsDoor3Unlocked)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsDoor3Unlocked, bInIsDoor3Unlocked);
}

bool UPS3ViewModel::GetIsDoor4Unlocked() const
{
	return bIsDoor4Unlocked;
}

void UPS3ViewModel::SetIsDoor4Unlocked(bool bInIsDoor4Unlocked)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsDoor4Unlocked, bInIsDoor4Unlocked);
}

void UPS3ViewModel::RequestDoorActivation(int32 InDoorIndex, bool bIsActive)
{
	if (InDoorIndex < 1 || InDoorIndex > 4)
	{
		return;
	}

	OnDoorActivationRequested_UI.Broadcast(InDoorIndex, bIsActive);
}

void UPS3ViewModel::RequestVoiceChatSpeaking(bool bInIsSpeaking)
{
	if (!bVoiceChatIconEnabled)
	{
		RequestSetVoiceChatIconVisible(false);
		return;
	}

	if (PlayerHUD)
	{
		PlayerHUD->UpdateVoiceChatIcon(bInIsSpeaking);
	}
}

void UPS3ViewModel::RequestSetVoiceChatIconVisible(bool bVisible)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetVoiceChatIconVisible(bVisible);
	}
}

void UPS3ViewModel::RequestSetOptionPopupVisible(bool bVisible)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetOptionPopupVisible(bVisible);
	}
}

void UPS3ViewModel::RequestToggleOptionPopup()
{
	if (PlayerHUD)
	{
		PlayerHUD->ToggleOptionPopup();
	}
}

void UPS3ViewModel::RequestExitToMain()
{
	OnExitToMainRequested_UI.Broadcast();
}

void UPS3ViewModel::RequestBGMVolumeChanged(float Value)
{
	OnBGMVolumeChanged_UI.Broadcast(Value);
}

void UPS3ViewModel::RequestSFXVolumeChanged(float Value)
{
	OnSFXVolumeChanged_UI.Broadcast(Value);
}

void UPS3ViewModel::RequestVoiceChatEnabledChanged(bool bEnabled)
{
	OnVoiceChatEnabledChanged_UI.Broadcast(bEnabled);
}

void UPS3ViewModel::RequestResolutionChanged(const FString& Resolution)
{
	OnResolutionChanged_UI.Broadcast(Resolution);
}

void UPS3ViewModel::RequestSetTitleVisible(bool bVisible)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetTitleVisible(bVisible);
	}
}

void UPS3ViewModel::RequestGameStart()
{
	OnGameStartRequested_UI.Broadcast();
}

void UPS3ViewModel::RequestGameExit()
{
	OnGameExitRequested_UI.Broadcast();
}

void UPS3ViewModel::RequestSetGameOverVisible(bool bVisible)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetGameOverVisible(bVisible);
	}
}

void UPS3ViewModel::RequestGameRestart()
{
	OnGameRestartRequested_UI.Broadcast();
}

void UPS3ViewModel::RequestSetStage5RoleSelectVisible(bool bVisible)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetStage5RoleSelectVisible(bVisible);
	}
}

void UPS3ViewModel::RequestStage5RoleSelection(EPS3PlayerRole SelectedRole)
{
	OnStage5RoleSelectionRequested_UI.Broadcast(SelectedRole);
}

bool UPS3ViewModel::GetIsOpen() const
{
	return bIsOpen;
}

void UPS3ViewModel::SetIsOpen(bool bInIsOpen)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsOpen, bInIsOpen);
}
