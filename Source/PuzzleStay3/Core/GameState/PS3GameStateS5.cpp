#include "PS3GameStateS5.h"

#include "Core/GameMode/PS3GameModeS5.h"
#include "Data/DataAsset/S5_GameRuleDataAsset.h"
#include "Data/Delegates/UIDelegatesSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/Character/PS3PlayerCharacter.h"
#include "Player/Controller/PS3PlayerController.h"
#include "Player/Controller/PS3PlayerControllerBase.h"
#include "Player/Controller/PS3ScreenPlayerController.h"


APS3GameStateS5::APS3GameStateS5()
{
	bReplicates = true;
}


void APS3GameStateS5::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeToDataAssets();
	
	PS3_UIDELEGATE_TIMER_FOR_MACRO(PS3_BROADCAST_TO_MVVM_OneParams(OnStageType_UI, S5_GameRuleDataAsset->StageType_S5));
	
	NoneReceivedDecalFloor();
	
	auto* PS3GameModeS5 = Cast<APS3GameModeS5>(GetWorld()->GetAuthGameMode());
	if (IsValid(PS3GameModeS5) == false) return;
	PS3GameModeS5->OnIsGameStart.AddUObject(this, &ThisClass::OnGameStart);
}


void APS3GameStateS5::InitializeToDataAssets()
{
	if (HasAuthority() == true)
	{
		checkf(IsValid(S5_GameRuleDataAsset) == true, TEXT("[APS3GameStateS5]의 데이터어셋이 비어있습니다."));
		GameLimitTime = S5_GameRuleDataAsset->MaxGameLimitTime;
	}
}


void APS3GameStateS5::OnSpawnScreenPlayerUIReAssign()
{
	if(HasAuthority() == true)
	{
		NetMultiRPC_OnSpawnScreenPlayerUIReAssign();
	}
}

void APS3GameStateS5::NetMultiRPC_OnSpawnScreenPlayerUIReAssign_Implementation()
{
	PS3_BROADCAST_TO_MVVM_OneParams(OnFieldPlayer_UI, true);
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
	DOREPLIFETIME(ThisClass, bIsGameStarted);
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
	
	auto* PS3PlayerControllerBase = Cast<APS3PlayerControllerBase>(PlayerController);
	if (IsValid(PS3PlayerControllerBase))
	{
		FInputModeUIOnly InputMode;
		PS3PlayerControllerBase->SetInputMode(InputMode);
		PS3PlayerControllerBase->bShowMouseCursor = true;
		PS3PlayerControllerBase->ConfigureInputMapping();
	}
	
	//PS3_BROADCAST_TO_MVVM_OneParams(OnVoiceChatIcon_UI, false);
	PS3_BROADCAST_TO_MVVM_OneParams(OnFieldPlayer_UI, false);
	PS3_BROADCAST_TO_MVVM_OneParams(OnScreenPlayer_UI, false);
	
	PS3_BROADCAST_TO_MVVM_OneParams(OnIsGameOver_UI, true);
}


void APS3GameStateS5::OnRep_GameLimitTime()
{
	if (bIsGameStarted == false) return;
	
	PS3_BROADCAST_TO_MVVM_TwoParams(OnGameTimer_UI, S5_GameRuleDataAsset->GameStartTimerUIType, GameLimitTime);
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

void APS3GameStateS5::OnGameStart(bool bIsGameStart)
{
	bIsGameStarted = bIsGameStart;
	
	ScreenPlayerVisibleToArrow();
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


void APS3GameStateS5::NoneReceivedDecalFloor()
{
	TArray<AActor*> FloorActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("NotDecalReceivedFloor"), FloorActors);

	checkf(FloorActors.Num() > 0, TEXT("바닥 메쉬 ActorTag 배열에 [NotDecalReceivedFloor] Tag를 추가해주세요."));
		
	for (AActor* Actor : FloorActors)
	{
		if (IsValid(Actor) == false) continue;
		
		UStaticMeshComponent* FloorMeshComp = Actor->FindComponentByClass<UStaticMeshComponent>();
		if (IsValid(FloorMeshComp) == false) continue;
		
		FloorMeshComp->SetReceivesDecals(false);
	}		
}

void APS3GameStateS5::ScreenPlayerVisibleToArrow()
{
	auto* PC = GetWorld()->GetFirstPlayerController();
	if (IsValid(PC) == false) return;
	
	auto* PS3FieldPlayerController = Cast<APS3PlayerController>(PC);
	if (IsValid(PS3FieldPlayerController) == false) return;
	
	auto* PS3FieldPlayer = Cast<APS3PlayerCharacter>(PS3FieldPlayerController->GetPawn());
	if (IsValid(PS3FieldPlayer) == false) return;
	
	auto* PS3ScreenPlayerController = Cast<APS3ScreenPlayerController>(PC);
	if (IsValid(PS3ScreenPlayerController) == false) return;
	
	if (PS3ScreenPlayerController->IsLocalPlayerController() == true)
	{
		PS3FieldPlayer->ArrowMeshComp->SetVisibility(true);
	}
}


