// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameModeS5.h"

#include "EngineUtils.h"
#include "Component/InteractionSwitchComponent.h"
#include "Core/GameState/PS3GameStateS5.h"
#include "Data/DataAsset/S5_GameRuleDataAsset.h"
#include "Data/Enum/PlayerStartType.h"
#include "Data/Enum/PS3PlayerRole.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Object/GimmickBase.h"
#include "Object/PS3PlayerStartBase.h"
#include "Player/Character/PS3PlayerCharacter.h"
#include "Player/Controller/PS3PlayerController.h"
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

void APS3GameModeS5::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	if (NewPlayer == nullptr) return;
	
	auto* PS3ChoiceController = Cast<APS3ChoiceController>(NewPlayer);
	if (IsValid(PS3ChoiceController) == true)
	{
		PS3ChoiceController->SetViewTarget(NewPlayer);
		
		return;
	}
		
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}

void APS3GameModeS5::BeginPlay()
{
	Super::BeginPlay();
	
	EscapeGimmickDetection();
}

void APS3GameModeS5::StageRestart()
{
	Super::StageRestart();
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
		
		APawn* OldPawn = OldController->GetPawn();
		if (IsValid(OldPawn) == true)
		{
			OldController->UnPossess();
			OldPawn->Destroy();
		}
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

void APS3GameModeS5::EscapeGimmickDetection()
{
	TargetEscapeGimmickArray.Empty();
	
	TArray<AActor*> GimmickBaseActorArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGimmickBase::StaticClass(), GimmickBaseActorArray);

	for (AActor* GimmickBaseActor : GimmickBaseActorArray)
	{
		auto* InstancedGimmickBaseActor = Cast<AGimmickBase>(GimmickBaseActor);
		if (IsValid(InstancedGimmickBaseActor) == false) continue;
		
		if (InstancedGimmickBaseActor->ActorHasTag(EscapeGimmickTagName) == true)
		{
			auto* InstancedEscapeSwitchComp = InstancedGimmickBaseActor->FindComponentByClass<UInteractionSwitchComponent>();
			if (IsValid(InstancedEscapeSwitchComp) == false) continue;
			
			TargetEscapeGimmickArray.Add(InstancedEscapeSwitchComp);
		}
		else UE_LOG(LogTemp, Error, TEXT("EscapeGimmick의 감지 된 Tag와 (%s)Tag 불일치"), *EscapeGimmickTagName.ToString());
	}

	int32 ComponentCount = TargetEscapeGimmickArray.Num();
	UE_LOG(LogTemp, Warning, TEXT("감지된 EscapeGimmick 컴포넌트 개수: %d개"), ComponentCount);
}

void APS3GameModeS5::OnEscapeGimmickUnlocked()
{
	int32 SuccessConditionsNumber = 2;
	
	if (TargetEscapeGimmickArray.Num() < SuccessConditionsNumber) return;
	if (LoginUserArray.Num() < SuccessConditionsNumber) return;
	
	if (TargetEscapeGimmickArray.Num() >= (LoginUserArray.Num()-1))
	{
		auto* PS3ScreenPlayerController = Cast<APS3ScreenPlayerController>(GetWorld()->GetFirstPlayerController());
		ConfigureControllerAndSpawn(PS3ScreenPlayerController,S5_GameRuleDataAsset->FieldControllerClass);
	}
	
	if (TargetEscapeGimmickArray.Num() >= LoginUserArray.Num())
	{
		//TODO 다음스테이지 입장하는 부분 구현해야함
		UE_LOG(LogTemp, Warning, TEXT("구현 예정 기능 예시) 5초 뒤 다음 스테이지 입장."));
	}
}

void APS3GameModeS5::CollectLoginUser()
{
	LoginUserArray.Empty();
	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* LoginUser = It->Get();
		if (IsValid(LoginUser) == false) continue;
		
		LoginUserArray.Add(LoginUser);
		
		int32 Count = LoginUserArray.Num();
		UE_LOG(LogTemp, Warning, TEXT("현재 로그인 인원: %d명"), Count);
	}
}

void APS3GameModeS5::SetPlayerControllerRole(APlayerController* CurrentController, EPS3PlayerRole SelectedPlayerRoleType)
{
	if (IsValid(CurrentController) == false) return;
	
	//TODO 디버그 매세지 나중에 삭제하기
	FString ControllerName = CurrentController->GetName();
	if (SelectedPlayerRoleType == EPS3PlayerRole::Field && bIsTakeFieldControllerType == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("3인칭조작 컨트롤러는 [%s]에게 이미 할당 되어있습니다."), *ControllerName);
		return;
	}
	
	if (SelectedPlayerRoleType == EPS3PlayerRole::Screen && bIsTakeScreenControllerType == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("스크린조작 컨트롤러는 [%s]에게 이미 할당 되어있습니다."), *ControllerName);
		return; 
	}
	
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

	if (InController->IsA(S5_GameRuleDataAsset->ScreenControllerClass) == true)
	{
		return Super::GetDefaultPawnClassForController_Implementation(InController);
	}

	// 2. 순수 필드 컨트롤러인 경우 -> FieldCharacterClass 반환
	if (InController->GetClass() == S5_GameRuleDataAsset->FieldControllerClass)
	{
		return S5_GameRuleDataAsset->FieldCharacterClass;
	}  
    
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

AActor* APS3GameModeS5::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	if (IsValid(Player) == false)
	{
		return Super::FindPlayerStart_Implementation(Player, IncomingName);
	}
	
	EPlayerStartType TargetPlayerStartType = EPlayerStartType::None;
	
	auto* FieldController = Cast<APS3PlayerController>(Player);
	auto* ScreenController = Cast<APS3ScreenPlayerController>(Player);
	if (Player->IsA(S5_GameRuleDataAsset->ScreenControllerClass) == true)
	{
		TargetPlayerStartType = EPlayerStartType::ScreenPlayer;
	}
	else if (Player->GetClass() == S5_GameRuleDataAsset->FieldControllerClass)
	{
		TargetPlayerStartType = EPlayerStartType::FieldPlayer;
	}
	
	
	for (TActorIterator<APS3PlayerStartBase> It(GetWorld()); It; ++It)
	{
		APS3PlayerStartBase* StartPoint = *It;
		if (IsValid(StartPoint) == true && StartPoint->PlayerStartType == TargetPlayerStartType) return StartPoint; 
	}
	
	return Super::FindPlayerStart_Implementation(Player, IncomingName);
}

void APS3GameModeS5::ConfigureControllerAndSpawn(APlayerController* OldController, TSubclassOf<APlayerController> NewControllerClass)
{
	if (IsValid(OldController) == false) return;
	if (NewControllerClass == nullptr) return;
	
	APawn* OldPawn = OldController->GetPawn();
	if (IsValid(OldPawn) == true)
	{
		OldController->UnPossess();
		OldPawn->Destroy();
	}
	
	FActorSpawnParameters ControllerSpawnParams;
	ControllerSpawnParams.Owner = this;
	auto* NewController = GetWorld()->SpawnActor<APlayerController>(NewControllerClass, ControllerSpawnParams);
	if (IsValid(NewController) == false) return;
	
	SwapPlayerControllers(OldController, NewController);
	OldController->Destroy();
	
	RestartPlayer(NewController);
}

