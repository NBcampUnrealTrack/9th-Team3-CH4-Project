#include "PS3GameStateS5.h"

#include "Core/GameMode/PS3GameModeS5.h"
#include "Data/DataAsset/S5_GameRuleDataAsset.h"
#include "Data/Delegates/UIDelegatesSubsystem.h"
#include "Data/Enum/PS3StageType.h"
#include "Net/UnrealNetwork.h"


APS3GameStateS5::APS3GameStateS5()
{
	bReplicates = true;
	
	
}


void APS3GameStateS5::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeToDataAssets();
	
	PS3_BROADCAST_TO_UI_OneParams(OnStageType_UI, S5_GameRuleDataAsset->StageType_S5);
	PS3_BROADCAST_TO_UI_OneParams(OnVoiceChatIcon_UI, true);
}


void APS3GameStateS5::InitializeToDataAssets()
{
	if (HasAuthority() == true)
	{
		checkf(IsValid(S5_GameRuleDataAsset) == true, TEXT("[APS3GameStateS5]의 데이터어셋이 비어있습니다."));
		GameLimitTime = S5_GameRuleDataAsset->MaxGameLimitTime;
	}
}

void APS3GameStateS5::InitializeBindFunction()
{
	if (HasAuthority() == false) return;
	
	if (IsValid(GetCastPS3GameModeS5()) == false) return;
	CastPS3GameModeS5->OnScreenPlayerSpawned.AddUObject(this, &APS3GameStateS5::OnScreenPlayerUI_Hide);
}


//Caching으로 컨트롤러를 넣진않는다.
APS3GameModeS5* APS3GameStateS5::GetCastPS3GameModeS5()
{
	if (HasAuthority() == false) return nullptr;
	
	if (IsValid(CastPS3GameModeS5) == false)
	{
		if (UWorld* World = GetWorld())
		{
			CastPS3GameModeS5 = Cast<APS3GameModeS5>(World->GetAuthGameMode());
		}
	}	
	
	return CastPS3GameModeS5;
}


void APS3GameStateS5::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, GameLimitTime);
	DOREPLIFETIME(ThisClass, bIsGameOver);
	DOREPLIFETIME(ThisClass, bIsSelectedFieldType);
	DOREPLIFETIME(ThisClass, bIsSelectedScreenType);
}


void APS3GameStateS5::ReSpawnPlayer(APlayerController* TargetPlayerController)
{
	if (HasAuthority() == true)
	{
		if (IsValid(TargetPlayerController) == false) return;
		if (IsValid(GetCastPS3GameModeS5()) == false) return;
		
		CastPS3GameModeS5->ReSpawnPlayer(TargetPlayerController);
	}
}


void APS3GameStateS5::OnRep_IsGameOver()
{
	if (bIsGameOver == false) return;
	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (IsValid(PlayerController))
	{
		FInputModeUIOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = true;
	}
	
	PS3_BROADCAST_TO_UI_OneParams(OnVoiceChatIcon_UI, false);
	PS3_BROADCAST_TO_UI_OneParams(OnFieldPlayer_UI, false);
	PS3_BROADCAST_TO_UI_OneParams(OnScreenPlayer_UI, false);
	PS3_BROADCAST_TO_UI_OneParams(OnIsGameOver_UI, true);
	
}


void APS3GameStateS5::OnRep_GameLimitTime()
{
	PS3_BROADCAST_TO_UI_TwoParams(OnGameTimer_UI, S5_GameRuleDataAsset->GameStartTimerUIType, S5_GameRuleDataAsset->MaxGameLimitTime);
	UE_LOG(LogTemp, Error, TEXT("(UI표시 업데이트 예정) 남은 제한시간: %f"), GameLimitTime);
}


void APS3GameStateS5::SetDeductGameLimitTime_AuthorityOnRep(float TimeToDeducted)
{
	if (HasAuthority() == true)
	{
		GameLimitTime = GameLimitTime - TimeToDeducted;
		OnRep_GameLimitTime(); //서버가 읽을때는 수동으로 OnRep를 써야함
	}
	
}


void APS3GameStateS5::NetMultiRPC_OnScreenPlayerUI_Hide_Implementation()
{
	PS3_BROADCAST_TO_UI_OneParams(OnScreenPlayer_UI, false);
}


void APS3GameStateS5::OnScreenPlayerUI_Hide()
{
	NetMultiRPC_OnScreenPlayerUI_Hide();
}


void APS3GameStateS5::SetIsGameOver_AuthorityOnRep(bool SetIsGameOver)
{
	if (HasAuthority() == true)
	{
		bIsGameOver = SetIsGameOver;
		OnRep_IsGameOver();
	}
}


void APS3GameStateS5::OnGameOver()
{
	SetIsGameOver_AuthorityOnRep(true);
}


void APS3GameStateS5::OnReduceGameTime(float ReducedTimeRange)
{
	SetDeductGameLimitTime_AuthorityOnRep(ReducedTimeRange);
}


void APS3GameStateS5::OnTimeDeduction(float TimeToDeducted)
{
	SetDeductGameLimitTime_AuthorityOnRep(TimeToDeducted);
}


void APS3GameStateS5::StageRestart()
{
	if (HasAuthority() == true)
	{
		if (IsValid(GetCastPS3GameModeS5()) == false) return;
	
		CastPS3GameModeS5->StageRestart();
	}
}


void APS3GameStateS5::OnQuitGame()
{
	if (HasAuthority() == true)
	{
		if (IsValid(GetCastPS3GameModeS5()) == false) return;
		
		CastPS3GameModeS5->OnQuitGame();
	}
}





