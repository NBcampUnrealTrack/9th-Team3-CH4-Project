// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameModeS5.h"

#include "Core/GameState/PS3GameStateS5.h"
#include "Data/Enum/PS3PlayerRole.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "Player/Character/PS3PlayerCharacter.h"
#include "PuzzleStay3/Core/GameInstance/PS3GameInstance.h"
#include "PuzzleStay3/Player/Controller/PS3ChoiceController.h"

void APS3GameModeS5::BeginPlay()
{
	Super::BeginPlay();
	
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
		PossessedControllerAndSpawn(CurrentController, FieldControllerClass, FieldCharacterClass);
		
	}
	
	if (CurrentPlayerRoleType == EPS3PlayerRole::Screen)
	{
		bIsTakeScreenControllerType = true;
		UE_LOG(LogTemp, Warning, TEXT("스크린조작 플레이어 생성"));
		PossessedControllerAndSpawn(CurrentController, ScreenControllerClass, ScreenCharacterClass);
		
	}	
	
	if (CurrentPlayerRoleType == EPS3PlayerRole::Unassigned)
	{
		return;
	}
	
}

void APS3GameModeS5::PossessedControllerAndSpawn(APlayerController* OldController, 
	TSubclassOf<APlayerController> NewControllerClass, TSubclassOf<APS3PlayerCharacter> NewCharacterClass)
{
	if (IsValid(OldController) == false) return;
	if (NewControllerClass == nullptr) return;
	if (NewCharacterClass == nullptr) return;
	UE_LOG(LogTemp, Warning, TEXT("컨트롤러 클래스 삽입 완료... 다음 로직 수행 가능..."));
	
	FString TargetTag = TEXT("");
	
	if (NewControllerClass == FieldControllerClass)
	{
		TargetTag = FieldTagString;
	}
	
	else if (NewControllerClass == ScreenControllerClass)
	{
		TargetTag = ScreenTagString;
	}
	
	//bool bWasLocalController = OldController->IsLocalController();
	
	
	APawn* OldPawn = OldController->GetPawn();
	if (IsValid(OldPawn) == true)
	{
		OldController->UnPossess();
		OldPawn->Destroy();
	}
	
	
	FActorSpawnParameters ControllerSpawnParams;
	auto* NewController = GetWorld()->SpawnActor<APlayerController>(NewControllerClass, ControllerSpawnParams);
	if (IsValid(NewController) == false) return;
	
	
	/*if (bWasLocalController == true)
	{
		NewController->SetAsLocalPlayerController();
	}*/
	
	SwapPlayerControllers(OldController, NewController);
	
	/*if (bWasLocalController == true)
	{
		FInputModeGameOnly InputMode;
		NewController->SetInputMode(InputMode);
		NewController->bShowMouseCursor = false;

		NewController->SetIgnoreMoveInput(false);
		NewController->SetIgnoreLookInput(false);

		NewController->InitInputSystem();
	}*/
	
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
	
	UE_LOG(LogTemp, Warning, TEXT("플레이어 스폰 완료"));
	
}




