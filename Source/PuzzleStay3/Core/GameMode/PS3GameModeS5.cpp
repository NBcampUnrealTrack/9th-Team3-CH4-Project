// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameModeS5.h"

#include "Component/InteractionSwitchComponent.h"
#include "Core/GameState/PS3GameStateS5.h"
#include "Data/DataAsset/Stage5ControllerDataAsset.h"
#include "Data/Enum/PS3PlayerRole.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Object/GimmickBase.h"
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
		
	PS3GameStateS5->OnReduceGameTime(ReducedTimeRange);
	
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
	EscapeGimmickArray.Empty();
	
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
			
			EscapeGimmickArray.Add(InstancedEscapeSwitchComp);
		}
		else UE_LOG(LogTemp, Error, TEXT("EscapeGimmick의 감지 된 Tag와 (%s)Tag 불일치"), *EscapeGimmickTagName.ToString());
	}

	int32 ComponentCount = EscapeGimmickArray.Num();
	UE_LOG(LogTemp, Warning, TEXT("감지된 EscapeGimmick 컴포넌트 개수: %d개"), ComponentCount);
}

void APS3GameModeS5::OnEscapeGimmickUnlocked()
{
	int32 SuccessConditionsNumber = 2;
	
	if (EscapeGimmickArray.Num() < SuccessConditionsNumber) return;
	if (LoginUserArray.Num() < SuccessConditionsNumber) return;
	
	if (EscapeGimmickArray.Num() >= (LoginUserArray.Num()-1))
	{
		auto* PS3ScreenPlayerController = Cast<APS3ScreenPlayerController>(GetWorld()->GetFirstPlayerController());
		ScreenPlayerSpawnCharacter(PS3ScreenPlayerController, Stage5ControllerDataAsset->FieldControllerClass, Stage5ControllerDataAsset->FieldCharacterClass, Stage5ControllerDataAsset->ScreenPlayerTagString);
	}
	
	if (EscapeGimmickArray.Num() >= LoginUserArray.Num())
	{
		//TODO 다음스테이지 입장하는 부분 구현해야함
		UE_LOG(LogTemp, Warning, TEXT("구현 예정 기능 예시) 5초 뒤 다음 스테이지 입장."));
	}
}

void APS3GameModeS5::OnCollectLoginUser()
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
	FString ControllerName = CurrentController->GetName(); // 예: PS3ChoiceController_0, PS3ChoiceController_1
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
		ConfigureControllerAndSpawn(CurrentController, Stage5ControllerDataAsset->FieldControllerClass, Stage5ControllerDataAsset->FieldCharacterClass);
		
	}
	
	if (CurrentPlayerRoleType == EPS3PlayerRole::Screen)
	{
		bIsTakeScreenControllerType = true;
		UE_LOG(LogTemp, Warning, TEXT("스크린조작 플레이어 생성"));
		ConfigureControllerAndSpawn(CurrentController, Stage5ControllerDataAsset->ScreenControllerClass, nullptr);
	}	
	
	if (CurrentPlayerRoleType == EPS3PlayerRole::Unassigned)
	{
		return;
	}
	
}



void APS3GameModeS5::ConfigureControllerAndSpawn(
		APlayerController* OldController, 
		TSubclassOf<APlayerController> NewControllerClass, 
		TSubclassOf<APS3PlayerCharacter> NewCharacterClass)
{
	if (IsValid(OldController) == false) return;
	if (NewControllerClass == nullptr) return;
	if (NewCharacterClass == nullptr) return;
	UE_LOG(LogTemp, Warning, TEXT("컨트롤러 클래스 삽입 완료... 다음 로직 수행 가능..."));
	
	APawn* OldPawn = OldController->GetPawn();
	if (IsValid(OldPawn) == true)
	{
		OldController->UnPossess();
		OldPawn->Destroy();
	}
	
	if (NewControllerClass == Stage5ControllerDataAsset->FieldControllerClass)
	{
		FieldPlayerConfigureAndSpawn(OldController, NewControllerClass, NewCharacterClass, Stage5ControllerDataAsset->FieldPlayerTagString);
	}
	else if (NewControllerClass == Stage5ControllerDataAsset->ScreenControllerClass)
	{
		ScreenPlayerConfigure(OldController, NewControllerClass);
	}
}

void APS3GameModeS5::FieldPlayerConfigureAndSpawn(
	APlayerController* OldController, 
	TSubclassOf<APlayerController> NewControllerClass, 
	TSubclassOf<APS3PlayerCharacter> NewCharacterClass,
	FString TargetTag)
{
	FActorSpawnParameters ControllerSpawnParams;
	auto* NewController = GetWorld()->SpawnActor<APlayerController>(NewControllerClass, ControllerSpawnParams);
	if (IsValid(NewController) == false) return;
	
	SwapPlayerControllers(OldController, NewController);
	OldController->Destroy();
	
	AActor* GameStartPoint = FindPlayerStart(NewController, TargetTag);
	if (IsValid(GameStartPoint) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("스타트지점 액터가 배치되어있지 않거나 Tag가 비어있습니다."));
		UE_LOG(LogTemp, Warning, TEXT("현재 비어있는 PlayerStartTag: %s"), *TargetTag);
		return;
	}
	
	FTransform SpawnTransform = FTransform::Identity;
	
	auto* PlayerStartPoint = Cast<APlayerStart>(GameStartPoint);
	if (IsValid(PlayerStartPoint) == true)
	{
		SpawnTransform = PlayerStartPoint->GetActorTransform(); 
	}
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = NewController;
		
	//가능하면 겹치지 않는 주변 위치로 조금 비켜서(Adjust) 스폰하되, 정 안되더라도 무조건 스폰(Always Spawn)시켜라
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
	auto* NewCharacter = GetWorld()->SpawnActor<APS3PlayerCharacter>(NewCharacterClass, SpawnTransform, SpawnParams);
	if (IsValid(NewCharacter) == false) return;
		
	NewController->Possess(NewCharacter);
	
	UE_LOG(LogTemp, Warning, TEXT("3인칭 플레이어 컨트롤러 할당 및 캐릭터 스폰 완료"));
}

void APS3GameModeS5::ScreenPlayerConfigure(
	APlayerController* OldController, 
	TSubclassOf<APlayerController> NewControllerClass)
{
	FActorSpawnParameters ControllerSpawnParams;
	auto* NewController = GetWorld()->SpawnActor<APlayerController>(NewControllerClass, ControllerSpawnParams);
	if (IsValid(NewController) == false) return;
	
	SwapPlayerControllers(OldController, NewController);
	OldController->Destroy();
	
	UE_LOG(LogTemp, Warning, TEXT("스크린 플레이어 컨트롤러 할당 완료"));
}

void APS3GameModeS5::ScreenPlayerSpawnCharacter(APlayerController* OldController,
	TSubclassOf<APlayerController> NewControllerClass, TSubclassOf<APS3PlayerCharacter> NewCharacterClass,
	FString TargetTag)
{
	/*TODO 현재 플레이어 수 - (플레이어 수 -1) 만큼 문이 열리면 스크린 플레이어 컨트롤러 스왑 및 스폰 로직 구현하기
	CurrentController, FieldControllerClass, FieldCharacterClass, ScreenPlayerTagString*/
	
	
	if (IsValid(OldController) == false) return;
	if (NewControllerClass == nullptr) return;
	if (NewCharacterClass == nullptr) return;
	UE_LOG(LogTemp, Warning, TEXT("컨트롤러 클래스 삽입 완료... 다음 로직 수행 가능..."));
	
	APawn* OldPawn = OldController->GetPawn();
	if (IsValid(OldPawn) == true)
	{
		OldController->UnPossess();
		OldPawn->Destroy();
	}
	
	FActorSpawnParameters ControllerSpawnParams;
	auto* NewController = GetWorld()->SpawnActor<APlayerController>(NewControllerClass, ControllerSpawnParams);
	if (IsValid(NewController) == false) return;
	
	SwapPlayerControllers(OldController, NewController);
	OldController->Destroy();
	
	AActor* GameStartPoint = FindPlayerStart(NewController, TargetTag);
	if (IsValid(GameStartPoint) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("스타트지점 액터가 배치되어있지 않거나 Tag가 비어있습니다."));
		UE_LOG(LogTemp, Warning, TEXT("현재 비어있는 PlayerStartTag: %s"), *TargetTag);
		return;
	}
	
	FTransform SpawnTransform = FTransform::Identity;
	
	auto* PlayerStartPoint = Cast<APlayerStart>(GameStartPoint);
	if (IsValid(PlayerStartPoint) == true)
	{
		SpawnTransform = PlayerStartPoint->GetActorTransform(); 
	}
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = NewController;
		
	//가능하면 겹치지 않는 주변 위치로 조금 비켜서(Adjust) 스폰하되, 정 안되더라도 무조건 스폰(Always Spawn)시켜라
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
	auto* NewCharacter = GetWorld()->SpawnActor<APS3PlayerCharacter>(NewCharacterClass, SpawnTransform, SpawnParams);
	if (IsValid(NewCharacter) == false) return;
		
	NewController->Possess(NewCharacter);
	
	UE_LOG(LogTemp, Warning, TEXT("스크린 플레이어 캐릭터 스폰 완료"));
	
}


