// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameModeS5.h"

#include "Core/GameState/PS3GameState.h"
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
	GetWorld()->GetTimerManager().SetTimer(GameLimitTimeHandle, this, &ThisClass::OnReduceGameTime, 1.f, true);
}


void APS3GameModeS5::OnReduceGameTime()
{
	float CurrentGameLimitTime = GameLimitTime;
	
	--CurrentGameLimitTime;
	
	if (CurrentGameLimitTime <= 0.0f)
	{
		//TODO 나중에 게임모드베이스에서 GameOver 함수 추가하기
		UE_LOG(LogTemp, Error, TEXT("GameOver 예정"));
	}
}


void APS3GameModeS5::SetPlayerControllerRole(APlayerController* CurrentController, EPS3PlayerRoleType SelectedPlayerRoleType)
{
	if (IsValid(CurrentController) == false) return;
	
	//TODO 디버그 매세지 나중에 삭제하기
	FString ControllerName = CurrentController->GetName(); // 예: PS3ChoiceController_0, PS3ChoiceController_1
	if (SelectedPlayerRoleType == EPS3PlayerRoleType::PlayerRole_ThirdPerson && bIsTakeThirdPersonControllerType == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("3인칭조작 컨트롤러는 [%s]에게 이미 할당 되어있습니다."), *ControllerName);
		return;
	}
	
	if (SelectedPlayerRoleType == EPS3PlayerRoleType::PlayerRole_Screen && bIsTakeScreenControllerType == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("스크린조작 컨트롤러는 [%s]에게 이미 할당 되어있습니다."), *ControllerName);
		return; 
	}
	
	EPS3PlayerRoleType CurrentPlayerRoleType = SelectedPlayerRoleType;
	
	if (CurrentPlayerRoleType == EPS3PlayerRoleType::PlayerRole_ThirdPerson)
	{
		bIsTakeThirdPersonControllerType = true;
		UE_LOG(LogTemp, Warning, TEXT("3인칭 플레이어 생성"));
		PossessedControllerAndSpawn(CurrentController, ThirdPersonControllerClass, ThirdPersonCharacterClass);
		
	}
	
	if (CurrentPlayerRoleType == EPS3PlayerRoleType::PlayerRole_Screen)
	{
		bIsTakeScreenControllerType = true;
		UE_LOG(LogTemp, Warning, TEXT("스크린 플레이어 생성"));
		PossessedControllerAndSpawn(CurrentController, ScreenControllerClass, ScreenCharacterClass);
		
	}	
	
	if (CurrentPlayerRoleType == EPS3PlayerRoleType::PlayerRole_None)
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
	
	AActor* GameStartPoint = FindPlayerStart(NewController);
	if (IsValid(GameStartPoint) == false) return;
	
	auto* PlayerStartPoint = Cast<APlayerStart>(GameStartPoint);
	if (IsValid(PlayerStartPoint) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("스타트지점 액터를 배치 후 다시 시도하세요. (총 2개입니다.)"));
		UE_LOG(LogTemp, Warning, TEXT("Tag종류: ThirdPerson / Screen"));
		return;
	}
	
	if (NewControllerClass == ThirdPersonControllerClass)
	{
		if (PlayerStartPoint->PlayerStartTag.ToString() != ThirdPersonString)
		{
			UE_LOG(LogTemp, Warning, TEXT("스타트지점 액터의 Tag가 비어있습니다. (ThirdPersonPlayer)"));
			return;
		}
		
		GameStartPoint = FindPlayerStart(NewController, ThirdPersonString);
	}
	
	if (NewControllerClass == ScreenControllerClass)
	{
		if (PlayerStartPoint->PlayerStartTag.ToString() != ThirdPersonString)
		{
			UE_LOG(LogTemp, Warning, TEXT("스타트지점 액터의 Tag가 비어있습니다. (ScreenPlayer)"));
			return;
		}
		
		GameStartPoint = FindPlayerStart(NewController, ScreenString);
	}
	
	
	FTransform SpawnTransform = FTransform::Identity;
	if (IsValid(GameStartPoint) == true)
	{
		SpawnTransform = GameStartPoint->GetActorTransform(); 
	}
		
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = NewController;
		
	//가능하면 겹치지 않는 주변 위치로 조금 비켜서(Adjust) 스폰하되, 정 안되더라도 무조건 스폰(Always Spawn)시켜라
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
	auto* NewCharacter = GetWorld()->SpawnActor<APS3PlayerCharacter>(NewCharacterClass, SpawnTransform, SpawnParams);
	if (IsValid(NewCharacter) == false) return;
		
		
	NewController->Possess(NewCharacter);
	
}

