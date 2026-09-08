#include "PS3GameModeS5.h"

#include "EngineUtils.h"
#include "Algo/RandomShuffle.h"
#include "Component/InteractionSwitchComponent.h"
#include "Component/OverlapVolumeTimeDeductionComponent.h"
#include "Core/GameState/PS3GameStateS5.h"
#include "Data/DataAsset/S5_GameRuleDataAsset.h"
#include "Data/Enum/PlayerStartType.h"
#include "Data/Enum/PS3PlayerRole.h"
#include "Kismet/GameplayStatics.h"
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
	
	checkf(IsValid(S5_GameRuleDataAsset) == true, TEXT("[APS3GameModeS5]의 데이터어셋이 비어있습니다."));
	
	RandomInitializeEscapeDoor();
	
}


void APS3GameModeS5::RandomInitializeEscapeDoor()
{
	int32 AllEscapeDoorCount = OnCollectEscapeDoor();
	int32 MaxEscapeDoorCount = S5_GameRuleDataAsset->MaxEscapeDoorCount;
	
	if (AllEscapeDoorCount <= MaxEscapeDoorCount) return;
	
	Algo::RandomShuffle(GimmickBaseArray);
	
	int32 FakeEscapeDoorCount = 0;
	
	for (AGimmickBase* GimmickBase : GimmickBaseArray)
	{
		if (IsValid(GimmickBase) == false) continue;
		
		auto* InteractionSwitchComp = GimmickBase->FindComponentByClass<UInteractionSwitchComponent>();
		if (IsValid(InteractionSwitchComp) == false) continue;
	
		auto* TimeDeductionComp = GimmickBase->FindComponentByClass<UOverlapVolumeTimeDeductionComponent>();
		if (IsValid(TimeDeductionComp) == false) continue;
		
		RegisterInteractionSwitch(InteractionSwitchComp);
		
		InteractionSwitchComp->OnSwitchActivatedChanged.AddUObject(this, &ThisClass::OnInteractedEscapeDoor);
		
		InteractionSwitchComp->bIsEscapeDoor = false;
		TimeDeductionComp->bIsEscapeDoor = false;
		
		++FakeEscapeDoorCount;
		--GoalEscapeDoorCount;
		
		FString CompName = GimmickBase->GetName();
		FString TagName = GimmickBase->Tags.Num() > 0 ? GimmickBase->Tags[0].ToString() : TEXT("NoTag");
		UE_LOG(LogTemp, Warning, TEXT("감지된 Escape Door 중 감지 된 FakeDoor %d번 / %s - %s"), 
			 FakeEscapeDoorCount, *CompName, *TagName);
		
		if (AllEscapeDoorCount - FakeEscapeDoorCount == MaxEscapeDoorCount) return;
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


void APS3GameModeS5::UnPossessedAndDestroyOldPawn(APlayerController* OldPlayerController)
{
	APawn* OldPawn = OldPlayerController->GetPawn();
	if (IsValid(OldPawn) == true)
	{
		OldPlayerController->UnPossess();
		OldPawn->Destroy();
	}
}


void APS3GameModeS5::OnGameStart()
{
	OnIsGameStart.Broadcast(true);
	
	UE_LOG(LogTemp, Warning, TEXT("게임이 시작되었습니다."));
	GetWorld()->GetTimerManager().SetTimer(GameLimitTimeHandle, this, &ThisClass::OnReduceGameTime, 1.f, true);
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


void APS3GameModeS5::OnReduceGameTime()
{
	auto* PS3GameStateS5 = GetGameState<APS3GameStateS5>();
	if (IsValid(PS3GameStateS5) == false) return;
		
	PS3GameStateS5->OnReduceGameTime(S5_GameRuleDataAsset->ReducedTimeRange);
	
	if (PS3GameStateS5->GameLimitTime <= 0.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(GameLimitTimeHandle);
		UE_LOG(LogTemp, Warning, TEXT("제한시간 종료."));
		
		OnGameOver();
	}
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


int32 APS3GameModeS5::OnCollectEscapeDoor()
{
	GimmickBaseArray.Empty();
	GoalEscapeDoorCount = 0;
	
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGimmickBase::StaticClass(), FoundActors);
	
	for (AActor* Actor : FoundActors)
	{
		auto* GimmickBase = Cast<AGimmickBase>(Actor);
		if (IsValid(GimmickBase) == false) continue;
		
		auto* InteractionSwitchComp = GimmickBase->FindComponentByClass<UInteractionSwitchComponent>();
		if (IsValid(InteractionSwitchComp) == false) continue;
	
		auto* TimeDeductionComp = GimmickBase->FindComponentByClass<UOverlapVolumeTimeDeductionComponent>();
		if (IsValid(TimeDeductionComp) == false) continue;
		
		InteractionSwitchComp->bIsEscapeDoor = true;
		TimeDeductionComp->bIsEscapeDoor = true;
	
		GimmickBaseArray.Add(GimmickBase);
		++GoalEscapeDoorCount;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("감지된 EscapeDoor 총 %d개"), GimmickBaseArray.Num());
	return GimmickBaseArray.Num();
}


void APS3GameModeS5::OnInteractedEscapeDoor(bool bIsInteracted)
{
	if (bIsInteracted == false) return;
	
	UE_LOG(LogTemp, Warning, TEXT("상호작용 완료 됨"));
	++ActivatedEscapeDoorCount;
	
	int32 ScreenPlayerSpawnConditionCount = GoalEscapeDoorCount - (GoalEscapeDoorCount - 1);
	
	if (ActivatedEscapeDoorCount >= ScreenPlayerSpawnConditionCount && bIsScreenPlayerSpawnedField == false)
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
			bIsScreenPlayerSpawnedField = true;
			
			OnScreenPlayerSpawned.Broadcast();
			
			UE_LOG(LogTemp, Warning, TEXT("스크린컨트롤러스폰완료 됨"));
		}
		
		bIsScreenPlayerSpawnReady = false;
	}
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
	
	APawn* SpawnedPawn = NewController->GetPawn();
	if (IsValid(SpawnedPawn) == true)
	{
		FVector FinalLocation = SpawnedPawn->GetActorLocation();
		UE_LOG(LogTemp, Error, TEXT("[스폰 결과] 생성된 캐릭터: %s | 최종 위치: %s"), 
			*SpawnedPawn->GetName(), *FinalLocation.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[스폰 실패] NewController가 Pawn을 소유하지 못했습니다."));
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

