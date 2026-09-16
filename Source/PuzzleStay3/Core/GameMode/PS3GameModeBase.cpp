#include "PS3GameModeBase.h"

#include "EngineUtils.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Component/InteractionSwitchComponent.h"
#include "Core/GameInstance/PS3GameInstance.h"
#include "Core/GameState/PS3GameStateBase.h"
#include "Data/DataAsset/Base_GameRuleDataAsset.h"
#include "Data/Delegates/UIDelegatesSubsystem.h"
#include "Data/Enum/PlayerStartType.h"
#include "Object/PS3PlayerStartBase.h"
#include "Player/PlayerState/PS3PlayerState.h"


void APS3GameModeBase::BeginPlay()
{
	Super::BeginPlay();

	InitializeToDataAssets();
	
	//레벨 시작 시 플레이어의 사망 상태 변경 이벤트를 구독
	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		RegisterPlayerDeadState(Cast<APS3PlayerState>(PlayerState));
	}

	if (AllInteractionSwitchActivated())
	{
		OpenEscapeDoor();
	}
}

void APS3GameModeBase::PostLogin(APlayerController* NewPlayer)
{
	if (IsValid(NewPlayer))
	{
		if (APS3PlayerState* NewPlayerState = NewPlayer->GetPlayerState<APS3PlayerState>(); IsValid(NewPlayerState))
		{
			// Super::PostLogin에서 스폰하기 전에 시작점 선택에 사용할 P1/P2를 지정한다.
			SetPlayerIdentity(NewPlayerState);

			// 새 플레이어 접속 시 해당 플레이어의 사망 상태 변경 이벤트를 구독
			RegisterPlayerDeadState(NewPlayerState);
		}
	}

	Super::PostLogin(NewPlayer);
}

void APS3GameModeBase::InitializeToDataAssets()
{
	if(IsValid(Base_GameRuleDataAsset) == false) return;
	if (Base_GameRuleDataAsset->TitleLevel.IsNull()) return;
	
	TitleLevelPath = Base_GameRuleDataAsset->TitleLevel.ToSoftObjectPath().GetLongPackageName();

	StageClearDelay = Base_GameRuleDataAsset->StageClearDelay;
}

void APS3GameModeBase::SetPlayerIdentity(APS3PlayerState* NewPlayerState)
{
	// bool bPlayer1Taken = false;
	//
	// if (NewPlayerState->GetPlayerIdentity() != EPS3PlayerIdentity::Unassigned) return;
	//
	// for (APlayerState* PlayerState : GameState->PlayerArray)
	// {
	// 	APS3PlayerState* PS3PlayerState = Cast<APS3PlayerState>(PlayerState);
	//
	// 	if (!IsValid(PS3PlayerState)) continue;
	//
	// 	if (PS3PlayerState == NewPlayerState) continue;
	//
	// 	if (PS3PlayerState->GetPlayerIdentity() != EPS3PlayerIdentity::Player1) continue;
	//
	// 	bPlayer1Taken = true;
	// 	break;
	// }
	// if (bPlayer1Taken)
	// {
	// 	NewPlayerState->SetPlayerIdentity(EPS3PlayerIdentity::Player2);
	// }
	// else
	// {
	// 	NewPlayerState->SetPlayerIdentity(EPS3PlayerIdentity::Player1);
	// }	
	if (!IsValid(NewPlayerState)) return;

	if (NewPlayerState->GetPlayerIdentity() != EPS3PlayerIdentity::Unassigned) return;
	
	FString PlayerKey;

	if (NewPlayerState->GetUniqueId().IsValid())
	{
		PlayerKey = NewPlayerState->GetUniqueId()->ToString();
	}
	else
	{
		PlayerKey = NewPlayerState->GetPlayerName();
	}

	UPS3GameInstance* PS3GameInstance = GetGameInstance<UPS3GameInstance>();
	if (!IsValid(PS3GameInstance) || PlayerKey.IsEmpty()) return;

	EPS3PlayerIdentity CachedIdentity = EPS3PlayerIdentity::Unassigned;
	if (PS3GameInstance->TryGetCachedPlayerIdentity(PlayerKey, CachedIdentity))
	{
		NewPlayerState->SetPlayerIdentity(CachedIdentity);
		return;
	}

	bool bPlayer1Taken = false;

	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		const APS3PlayerState* PS3PlayerState = Cast<APS3PlayerState>(PlayerState);
		if (!IsValid(PS3PlayerState) || PS3PlayerState == NewPlayerState) continue;

		if (PS3PlayerState->GetPlayerIdentity() == EPS3PlayerIdentity::Player1)
		{
			bPlayer1Taken = true;
			break;
		}
	}

	EPS3PlayerIdentity NewIdentity = EPS3PlayerIdentity::Unassigned;

	if (bPlayer1Taken)
	{
		NewIdentity = EPS3PlayerIdentity::Player2;
	}
	else
	{
		NewIdentity = EPS3PlayerIdentity::Player1;
	}

	NewPlayerState->SetPlayerIdentity(NewIdentity);
	PS3GameInstance->CachePlayerIdentity(PlayerKey, NewIdentity);
}

AActor* APS3GameModeBase::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	if (!IsValid(Player))
	{
		return Super::FindPlayerStart_Implementation(Player, IncomingName);
	}

	APS3PlayerState* PS3PlayerState = Player->GetPlayerState<APS3PlayerState>();
	if (!IsValid(PS3PlayerState))
	{
		return Super::FindPlayerStart_Implementation(Player, IncomingName);
	}

	EPlayerStartType TargetStartType = EPlayerStartType::None;

	if (PS3PlayerState->GetPlayerIdentity() == EPS3PlayerIdentity::Player1)
	{
		TargetStartType = EPlayerStartType::Player1;
	}
	else if (PS3PlayerState->GetPlayerIdentity() == EPS3PlayerIdentity::Player2)
	{
		TargetStartType = EPlayerStartType::Player2;
	}

	for (TActorIterator<APS3PlayerStartBase> It(GetWorld()); It; ++It)
	{
		APS3PlayerStartBase* PlayerStart = *It;

		if (IsValid(PlayerStart) && PlayerStart->PlayerStartType == TargetStartType)
		{
			return PlayerStart;
		}
	}
	return Super::FindPlayerStart_Implementation(Player, IncomingName);
}

void APS3GameModeBase::RegisterInteractionSwitch(UInteractionSwitchComponent* SwitchComp)
{
	if (!IsValid(SwitchComp)) return;
	if (InteractionSwitches.Contains(SwitchComp)) return;

	InteractionSwitches.Add(SwitchComp);

	InteractionSwitchCompoHandle =
		SwitchComp->OnSwitchActivatedChanged.AddUObject(this, &APS3GameModeBase::HandleSwitchActivatedChanged);
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

void APS3GameModeBase::SetInteractionSwitchTimerUsed(bool IsUsed)
{
	bInteractionSwitchTimerUsed = IsUsed;
}

void APS3GameModeBase::HandleSwitchActivatedChanged(bool bActivated)
{
	OpenEscapeDoor();
}

void APS3GameModeBase::OpenEscapeDoor()
{
	if (AllInteractionSwitchActivated() == false) return;

	APS3GameStateBase* GS = GetGameState<APS3GameStateBase>();
	if (!IsValid(GS)) return;
	if (GS->IsEscapeDoorOpened()) return;

	GS->SetEscapeDoorOpened(true);

	CallStageClearIfTimerOver();
}

void APS3GameModeBase::DisableBlockingVolume(EPS3StageNumber StageNumber)
{
}

//플레이어 죽음 델리게이트 구독 함수
void APS3GameModeBase::RegisterPlayerDeadState(APS3PlayerState* PS3PlayerState)
{
	if (!IsValid(PS3PlayerState)) return;

	// PlayerState의 OnDeadStateChanged 델리게이트 시 HandlePlayerDeadState함수 호출
	PS3PlayerState->OnDeadStateChanged.AddUniqueDynamic(this, &APS3GameModeBase::HandlePlayerDeadState);
}

void APS3GameModeBase::HandlePlayerDeadState(bool bNewIsDead)
{
	if (!bNewIsDead) return;

	// 플레이어 사망 시 스테이지 재시작하지 않는 스테이지는 false로 두고 무시
	if (!StageRestartIfPlayerDead()) return;

	// bStageRestartRequested = true 일 경우 재시작하지 X
	if (bStageRestartRequested) return;
	bStageRestartRequested = true;
	
	APS3GameStateBase* GS = GetGameState<APS3GameStateBase>();
	if (!IsValid(GS)) return;

	GS->NotifyPlayerDeadGameOver();
	//StageRestart();ㄴ
}

void APS3GameModeBase::StageRestart()
{
	if (!HasAuthority()) return;

	//PlayerState의 IsDead 값을 False로 초기화
	ResetAllPlayersDeadState();

	FString CurrentLevel = UGameplayStatics::GetCurrentLevelName(this, true);

	if (CurrentLevel.IsEmpty()) return;

	GetWorld()->ServerTravel(CurrentLevel);
}

void APS3GameModeBase::ResetAllPlayersDeadState()
{
	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		APS3PlayerState* PS3PlayerState = Cast<APS3PlayerState>(PlayerState);
		if (!IsValid(PS3PlayerState)) continue;

		PS3PlayerState->SetIsDead(false);
	}
}

void APS3GameModeBase::StageClear()
{
	if (!HasAuthority()) return;
	if (NextStageLevelPath.IsEmpty()) return;

	GetWorld()->ServerTravel(NextStageLevelPath);
}

void APS3GameModeBase::CallStageClearIfTimerOver()
{
	if (!bStageClearTimerStarted)
	{
		bStageClearTimerStarted = true;

		UE_LOG(LogTemp, Warning, TEXT("Go to Next Stage After %f Seconds"), StageClearDelay);

		GetWorldTimerManager().SetTimer(
			StageClearTimerHandle,
			this,
			&APS3GameModeBase::StageClear,
			StageClearDelay,
			false
		);
	}
}

void APS3GameModeBase::GotoTitleLevel()
{
	if (!HasAuthority()) return;
	if (TitleLevelPath.IsEmpty()) return;

	GetWorld()->ServerTravel(TitleLevelPath);
}

