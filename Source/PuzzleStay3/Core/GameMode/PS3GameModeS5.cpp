#include "PS3GameModeS5.h"

#include "EngineUtils.h"
#include "Algo/RandomShuffle.h"
#include "Component/InteractionSwitchComponent.h"
#include "Component/OverlapVolumeTimeDeductionComponent.h"
#include "Component/S5_InteractionGimmickComponent.h"
#include "Core/GameState/PS3GameStateS5.h"
#include "Data/DataAsset/S5_GameRuleDataAsset.h"
#include "Data/Enum/PlayerStartType.h"
#include "Data/Enum/PS3PlayerRole.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Object/GimmickBase.h"
#include "Object/PS3PlayerStartBase.h"
#include "Player/Character/PS3PlayerCharacter.h"
#include "Player/Controller/PS3ScreenPlayerController.h"
#include "PuzzleStay3/Player/Controller/PS3ChoiceController.h"


APS3GameModeS5::APS3GameModeS5()
{
	DefaultPawnClass = nullptr;
}


void APS3GameModeS5::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	if (NewPlayer == nullptr) return;
	
	LoginUserArray.AddUnique(NewPlayer);
	
	auto* PS3ChoiceController = Cast<APS3ChoiceController>(NewPlayer);
	if (IsValid(PS3ChoiceController) == true)
	{
		PS3ChoiceController->SetOwner(PS3ChoiceController);
		
		UE_LOG(LogTemp, Warning, TEXT("[GameMode] ChoiceController PostLogin 완료: %s"), *PS3ChoiceController->GetName());
	}
}


void APS3GameModeS5::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeToDataAssets();
	GetWorld()->GetTimerManager().SetTimer(InitTimerHandle, this, &ThisClass::InitializeGimmick, 0.1f, false);
}


void APS3GameModeS5::InitializeToDataAssets()
{
	Super::InitializeToDataAssets();
	
	checkf(IsValid(S5_GameRuleDataAsset) == true, TEXT("[APS3GameModeS5]의 데이터어셋이 비어있습니다."));
	checkf(IsValid(S5_GameRuleDataAsset->FieldCharacterClass) == true, TEXT("[US5_GameRuleDataAsset]데이터어셋의 [FieldCharacterClass]가 비어있습니다."));
	checkf(IsValid(S5_GameRuleDataAsset->FieldControllerClass) == true, TEXT("[US5_GameRuleDataAsset]데이터어셋의 [FieldControllerClass]가 비어있습니다."));
	checkf(IsValid(S5_GameRuleDataAsset->ScreenControllerClass) == true, TEXT("[US5_GameRuleDataAsset]데이터어셋의 [ScreenControllerClass]가 비어있습니다."));
	checkf(IsValid(S5_GameRuleDataAsset->SpawnScreenControllerClass) == true, TEXT("[US5_GameRuleDataAsset]데이터어셋의 [SpawnScreenControllerClass]가 비어있습니다."));
	
	MaxPlayerCount = S5_GameRuleDataAsset->MaxPlayerCount;
	ReducedTimeRange = S5_GameRuleDataAsset->ReducedTimeRange;
	MaxEscapeDoorCount = S5_GameRuleDataAsset->MaxEscapeDoorCount;
	MaxInteractionGimmickCount = S5_GameRuleDataAsset->MaxInteractionGimmickCount;
	WaitingTime = S5_GameRuleDataAsset->WaitingTime;
}


void APS3GameModeS5::InitializeGimmick()
{
	GetWorld()->GetTimerManager().ClearTimer(InitTimerHandle);
	
	UnResistEscapeGimmick();
	RandomShuffleFakeGimmick();
	BindInteractionGimmick();
	ResistEscapeGimmick();
	
	UE_LOG(LogTemp, Warning, TEXT("활성화 해야 할 스크린플레이어 스폰 조건 %d개"), TargetCountForSpawnScreenPlayer);
}



int32 APS3GameModeS5::OnCollectGimmickBase()
{
	GimmickBaseArray.Empty();
	
	TargetCountForSpawnScreenPlayer = 0;
	
	for (TActorIterator<AGimmickBase> It(GetWorld()); It; ++It)
	{
		AGimmickBase* TargetGimmick = *It;
		
		if (IsValid(TargetGimmick) == false) continue;
		
		GimmickBaseArray.Add(TargetGimmick);
		
		auto* InteractionSwitchComp = TargetGimmick->FindComponentByClass<UInteractionSwitchComponent>();
		if (IsValid(InteractionSwitchComp) == true)
		{
			InteractionSwitchComp->bIsEscapeDoor = true;
		}
		
		auto* InteractionGimmickComp = TargetGimmick->FindComponentByClass<US5_InteractionGimmickComponent>();
		auto* TimeDeductionComp = TargetGimmick->FindComponentByClass<UOverlapVolumeTimeDeductionComponent>();
		if (IsValid(TimeDeductionComp) == true && IsValid(InteractionGimmickComp) == true)
		{
			OnIsInteractionGimmick.Broadcast(InteractionGimmickComp, true);
			OnIsInteractionGimmick.Broadcast(TimeDeductionComp, true);
			
			++InteractionGimmickCount;
		}
	}
	
	TargetCountForSpawnScreenPlayer = GimmickBaseArray.Num();
	
	UE_LOG(LogTemp, Warning, TEXT("감지된 GimmickBase 총 %d개"), GimmickBaseArray.Num());
	return GimmickBaseArray.Num();
}


void APS3GameModeS5::RandomShuffleFakeGimmick()
{
	int32 AllGimmickBaseCount = OnCollectGimmickBase();
	
	if (AllGimmickBaseCount <= MaxEscapeDoorCount) return;
	
	Algo::RandomShuffle(GimmickBaseArray);
	
	int32 IndexNumber = 0;
	int32 CurrentFakeGimmickCount = 0;
	const int32 TargetFakeGimmickCount = InteractionGimmickCount - MaxInteractionGimmickCount;
	
	for (AGimmickBase* TargetGimmick :GimmickBaseArray)
	{
		if (IsValid(TargetGimmick) == false) continue;
		
		auto* TimeDeductionComp = TargetGimmick->FindComponentByClass<UOverlapVolumeTimeDeductionComponent>();
		auto* InteractionGimmickComp = TargetGimmick->FindComponentByClass<US5_InteractionGimmickComponent>();
		
		if (IsValid(TimeDeductionComp) == true && IsValid(InteractionGimmickComp) == true)
		{
			OnIsInteractionGimmick.Broadcast(InteractionGimmickComp, false);
			OnIsInteractionGimmick.Broadcast(TimeDeductionComp, false);
			AssignFakeGimmickIDForUI(TimeDeductionComp, IndexNumber);
			
			++CurrentFakeGimmickCount;
			--TargetCountForSpawnScreenPlayer;
			
			FString TagName = TargetGimmick->Tags.Num() > 0 ? TargetGimmick->Tags[0].ToString() : TEXT("NoTag");
			UE_LOG(LogTemp, Warning, TEXT("\n[페이크 기믹] -> %s 현재 bool = %d"), *TagName, InteractionGimmickComp->bIsInteractionGimmick);
		}
		
		if (CurrentFakeGimmickCount >= TargetFakeGimmickCount) return;
	}
}


void APS3GameModeS5::BindInteractionGimmick()
{
	
	for (AGimmickBase* TargetGimmick :GimmickBaseArray)
	{
		if (IsValid(TargetGimmick) == false) continue;
		
		auto* InteractionSwitchComp = TargetGimmick->FindComponentByClass<UInteractionSwitchComponent>();
		if (IsValid(InteractionSwitchComp) == true) continue;
		
		auto* InteractionGimmickComp = TargetGimmick->FindComponentByClass<US5_InteractionGimmickComponent>();
		auto* TimeDeductionComp = TargetGimmick->FindComponentByClass<UOverlapVolumeTimeDeductionComponent>();
		
		if (IsValid(TimeDeductionComp) == false || IsValid(InteractionGimmickComp) == false) continue;
		
		if (InteractionGimmickComp->bIsInteractionGimmick == false 
			|| TimeDeductionComp->bIsInteractionGimmick == false) continue;
		
		if (InteractionGimmickComp->bIsInteractionGimmick == true && TimeDeductionComp->bIsInteractionGimmick == true)
		{
			OnIsInteractionGimmick.Broadcast(InteractionGimmickComp, true);
			OnIsInteractionGimmick.Broadcast(TimeDeductionComp, true);
			InteractionSwitchComp->OnInteractionSuccessed.AddUObject(this, &ThisClass::OnInteractedGimmick);
			
			FString TagName = TargetGimmick->Tags.Num() > 0 ? TargetGimmick->Tags[0].ToString() : TEXT("NoTag");
			UE_LOG(LogTemp, Warning, TEXT("\n[인터렉션 기믹] -> %s "), *TagName);
		}
	}
}



void APS3GameModeS5::ResistEscapeGimmick()
{
	for (TActorIterator<AGimmickBase> It(GetWorld()); It; ++It)
	{
		AGimmickBase* TargetGimmick = *It;
		if (IsValid(TargetGimmick) == false) continue;
		
		auto* InteractionSwitchComp = TargetGimmick->FindComponentByClass<UInteractionSwitchComponent>();
		if (IsValid(InteractionSwitchComp) == false) continue;
		
		if (InteractionSwitchComp->bIsEscapeDoor == true)
		{
			InteractionSwitchComp->bMultiInteractionState = false;
			InteractionSwitchComp->bIsOtherInteractionGimmick = false;
			
			RegisterInteractionSwitch(InteractionSwitchComp);
			
			--TargetCountForSpawnScreenPlayer;
			
			FString TagName = TargetGimmick->Tags.Num() > 0 ? TargetGimmick->Tags[0].ToString() : TEXT("NoTag");
			UE_LOG(LogTemp, Warning, TEXT("\n[탈출문 기믹] -> %s "), *TagName);
		}
	}
}


void APS3GameModeS5::OnInteractedGimmick(bool bIsInteractedGimmick)
{
	if (bIsInteractedGimmick == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnInteractedGimmick 함수 호출. 이미 눌린 기믹입니다."));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("OnInteractedGimmick 함수 호출. 상호작용 성공"));
	if (ActivatedInteractionGimmickCount < TargetCountForSpawnScreenPlayer)
	{
		++ActivatedInteractionGimmickCount;
		bIsScreenPlayerAlreadySpawned = false;
	}
	
	if (ActivatedInteractionGimmickCount >= TargetCountForSpawnScreenPlayer && bIsScreenPlayerAlreadySpawned == false)
	{
		
		
		TArray<APS3ScreenPlayerController*> TargetController;
		
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PlayerController = It->Get();
			if (IsValid(PlayerController) == false) continue;
			
			auto* ScreenPlayerController = Cast<APS3ScreenPlayerController>(PlayerController);
			if (IsValid(ScreenPlayerController) == false) continue;
			
			TargetController.Add(ScreenPlayerController);
		}
		
		bIsScreenPlayerSpawnReady = true;
		
		for (APS3ScreenPlayerController* ScreenPlayerController : TargetController)
		{
			if (IsValid(ScreenPlayerController) == false) continue;
			
			ConfigureControllerAndSpawn(ScreenPlayerController, S5_GameRuleDataAsset->SpawnScreenControllerClass);
			bIsScreenPlayerAlreadySpawned = true;
			
			break;
		}
		
		if (bIsScreenPlayerAlreadySpawned == true)
		{
			OnScreenPlayerSpawned.Broadcast();
			
			auto* PS3GameStateS5 = Cast<APS3GameStateS5>(GetWorld()->GetGameState());
			if (IsValid(PS3GameStateS5) == false) return;
		
			PS3GameStateS5->OnSpawnScreenPlayerUIReAssign();
		}
		
		bIsScreenPlayerSpawnReady = false;
	}
}




void APS3GameModeS5::UnResistEscapeGimmick()
{
	TArray<UInteractionSwitchComponent*> TempSwitches = InteractionSwitches;
	for (UInteractionSwitchComponent* OldInteractionSwitchComp : TempSwitches)
	{
		UnregisterInteractionSwitch(OldInteractionSwitchComp);
	}
	InteractionSwitches.Empty();
}

void APS3GameModeS5::AssignFakeGimmickIDForUI(class UOverlapVolumeTimeDeductionComponent* TimeDeductionComp, int32 IndexNumber)
{
	if (TimeDeductTimerUITypeArray.IsValidIndex(IndexNumber) == false) return;
	TimeDeductionComp->TimeDeductTimerUIType = TimeDeductTimerUITypeArray[IndexNumber];
}


void APS3GameModeS5::ConfigureControllerAndSpawn(APlayerController* OldController, TSubclassOf<APlayerController> NewControllerClass)
{
	if (IsValid(OldController) == false) return;
	if (NewControllerClass == nullptr) return;
	
	UnPossessedAndDestroyOldPawn(OldController);
	
	FActorSpawnParameters ControllerSpawnParams;
	ControllerSpawnParams.Owner = this;
	auto* NewController = GetWorld()->SpawnActor<APlayerController>(NewControllerClass, ControllerSpawnParams);
	if (IsValid(NewController) == false) return;
	
	SwapPlayerControllers(OldController, NewController);
	OldController->Destroy();
	
	AActor* TargetPlayerStart = FindPlayerStart(NewController);
	if (IsValid(TargetPlayerStart) == true)
	{
		NewController->SetInitialLocationAndRotation(TargetPlayerStart->GetActorLocation(), TargetPlayerStart->GetActorRotation());
		NewController->SetControlRotation(TargetPlayerStart->GetActorRotation());
	}
	
	RestartPlayer(NewController);
}


void APS3GameModeS5::UnPossessedAndDestroyOldPawn(APlayerController* OldPlayerController)
{
	APawn* OldPawn = OldPlayerController->GetPawn();
	if (IsValid(OldPawn) == true)
	{
		OldPlayerController->UnPossess();
		OldPawn->Destroy();
	}
}


void APS3GameModeS5::StageRestart()
{
	Super::StageRestart();
}


void APS3GameModeS5::ReSpawnPlayer(APlayerController* TargetPlayerController)
{
	if (IsValid(TargetPlayerController) == false) return;
	
	UnPossessedAndDestroyOldPawn(TargetPlayerController);
	RestartPlayer(TargetPlayerController);
}


void APS3GameModeS5::OnQuitGame()
{
	APlayerController* CurrentPlayer = GetWorld()->GetFirstPlayerController();
	if (CurrentPlayer == nullptr) return;
	
#if WITH_EDITOR
	// 에디터 실행 중(PIE 모드)일 경우: PIE 에디터 세션을 강제로 종료시킴
	CurrentPlayer->ConsoleCommand("Exit");
#else
	// 실제 빌드된 게임(.exe)일 경우: 정석적으로 시스템 종료
	UKismetSystemLibrary::QuitGame(
		GetWorld(),
		CurrentPlayer,
		EQuitPreference::Quit,
		false
	);
#endif
	
}


void APS3GameModeS5::OnTimeDeduction(float TimeToDeducted)
{
	auto* PS3GameStateS5 = GetGameState<APS3GameStateS5>();
	if (IsValid(PS3GameStateS5) == false) return;
	
	PS3GameStateS5->OnTimeDeduction(TimeToDeducted);
}


void APS3GameModeS5::OnCollectLoginUser()
{
	LoginUserArray.Empty();
	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* LoginUser = It->Get();
		if (IsValid(LoginUser) == false) continue;
		
		LoginUserArray.Add(LoginUser);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("현재 로그인 인원: %d명"), LoginUserArray.Num());
}


void APS3GameModeS5::SetPlayerControllerRole(APlayerController* CurrentController, EPS3PlayerRole SelectedPlayerRoleType)
{
	if (IsValid(CurrentController) == false) return;
	
	if (SelectedPlayerRoleType == EPS3PlayerRole::Field && bIsTakeFieldControllerType == true) return;
	if (SelectedPlayerRoleType == EPS3PlayerRole::Screen && bIsTakeScreenControllerType == true) return;
	
	++RoleSelectedPlayerCount;
	
	EPS3PlayerRole CurrentPlayerRoleType = SelectedPlayerRoleType;
	if (CurrentPlayerRoleType == EPS3PlayerRole::Field)
	{
		bIsTakeFieldControllerType = true;
		UE_LOG(LogTemp, Warning, TEXT("3인칭조작 플레이어 생성"));
		ConfigureControllerAndSpawn(CurrentController, S5_GameRuleDataAsset->FieldControllerClass);
		
	}
	else if (CurrentPlayerRoleType == EPS3PlayerRole::Screen)
	{
		bIsTakeScreenControllerType = true;
		UE_LOG(LogTemp, Warning, TEXT("스크린조작 플레이어 생성"));
		ConfigureControllerAndSpawn(CurrentController, S5_GameRuleDataAsset->ScreenControllerClass);
	}	
}


UClass* APS3GameModeS5::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (IsValid(InController) == false || S5_GameRuleDataAsset == nullptr)
	{
		return Super::GetDefaultPawnClassForController_Implementation(InController);
	}

	if (InController->IsA(S5_GameRuleDataAsset->SpawnScreenControllerClass) == true ||
		InController->IsA(S5_GameRuleDataAsset->FieldControllerClass) == true)
	{
		return S5_GameRuleDataAsset->FieldCharacterClass;
	}

	if (InController->IsA(S5_GameRuleDataAsset->ScreenControllerClass) == true)
	{
		return nullptr;
	}  
	
    
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}


AActor* APS3GameModeS5::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	if (IsValid(Player) == false || IsValid(S5_GameRuleDataAsset) == false)
	{
		return Super::FindPlayerStart_Implementation(Player, IncomingName);
	}
	
	EPlayerStartType TargetPlayerStartType = EPlayerStartType::None;
	
	if (bIsScreenPlayerSpawnReady == true)
	{
		TargetPlayerStartType = EPlayerStartType::ScreenPlayer;
	}
	else if (Player->IsA(S5_GameRuleDataAsset->FieldControllerClass))
	{
		TargetPlayerStartType = EPlayerStartType::FieldPlayer;
	}
	else
	{
		TargetPlayerStartType = EPlayerStartType::ScreenPlayer;
	}
	
	for (TActorIterator<APS3PlayerStartBase> It(GetWorld()); It; ++It)
	{
		APS3PlayerStartBase* PS3PlayerStartBase = *It;
		if (IsValid(PS3PlayerStartBase) == true && PS3PlayerStartBase->PlayerStartType == TargetPlayerStartType)
		{
			return PS3PlayerStartBase; 
		}
	}
	
	return Super::FindPlayerStart_Implementation(Player, IncomingName);
}


void APS3GameModeS5::OnTimerForGameStart()
{
	GetWorld()->GetTimerManager().SetTimer(TimerForGameStartHandle, this, &ThisClass::OnGameStart, WaitingTime, false);
}


void APS3GameModeS5::OnGameStart()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerForGameStartHandle);
	
	OnIsGameStart.Broadcast(true);
	
	UE_LOG(LogTemp, Warning, TEXT("게임이 시작되었습니다."));
	GetWorld()->GetTimerManager().SetTimer(GameLimitTimeHandle, this, &ThisClass::OnReduceGameTime, ReducedTimeRange, true);
}


void APS3GameModeS5::OnReduceGameTime()
{
	auto* PS3GameStateS5 = GetGameState<APS3GameStateS5>();
	if (IsValid(PS3GameStateS5) == false) return;
		
	PS3GameStateS5->OnReduceGameTime(ReducedTimeRange);
	
	if (PS3GameStateS5->GameLimitTime <= 0.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(GameLimitTimeHandle);
		UE_LOG(LogTemp, Warning, TEXT("제한시간 종료."));
		
		OnGameOver();
	}
}


void APS3GameModeS5::OnGameOver()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		auto* OldController = Cast<APlayerController>(*It);
		if (IsValid(OldController) == false) continue;
		
		UnPossessedAndDestroyOldPawn(OldController);
	}
	
	auto* PS3GameStateS5 = GetGameState<APS3GameStateS5>();
	if (IsValid(PS3GameStateS5) == false) return;
	
	OnIsGameStart.Broadcast(false);
	PS3GameStateS5->OnGameOver();
}