#include "PS3ScreenPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Data/Enum/ControlDoorType.h"
#include "Kismet/GameplayStatics.h"
#include "Object/ControlDoor.h"
#include "Object/PS3CameraActor.h"

APS3ScreenPlayerController::APS3ScreenPlayerController()
{
	bShowMouseCursor = true;
}

void APS3ScreenPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	ConfigureLocalInputMode();
	
	UWorld* World = GetWorld();
	if (IsValid(World) == false) return;
	World->GetTimerManager().SetTimer(PS3CameraTimerHandle, this, &ThisClass::SetCameraView, 0.1f, false);
}

bool APS3ScreenPlayerController::IsScreenPlayer() const
{
	return true;
}



//TODO 추가 사항 확인 필요: 배치 된 Door액터 배열 보관 - 김명현
void APS3ScreenPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> ActorArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AControlDoor::StaticClass(),ActorArray);
	
	for (AActor* Actor : ActorArray)
	{
		auto* ControlDoor = Cast<AControlDoor>(Actor);
		if (IsValid(ControlDoor) == false) continue;
		
		ControlDoorArray.Add(ControlDoor);
	}
	
	
	
}

//TODO 추가 사항 확인 필요: 키바인딩 추가 - 김명현
void APS3ScreenPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (!IsValid(EIC))
	{
		return;
	}
	
	if (IsValid(Button_A))
	{
		EIC->BindAction(Button_A, ETriggerEvent::Started, this, &ThisClass::OpenDoor_A);
		EIC->BindAction(Button_A, ETriggerEvent::Completed, this, &ThisClass::CloseDoor_A);
	}
	if (IsValid(Button_B))
	{
		EIC->BindAction(Button_B, ETriggerEvent::Started, this, &ThisClass::OpenDoor_B);
		EIC->BindAction(Button_B, ETriggerEvent::Completed, this, &ThisClass::CloseDoor_B);
	}
	if (IsValid(Button_C))
	{
		EIC->BindAction(Button_C, ETriggerEvent::Started, this, &ThisClass::OpenDoor_C);
		EIC->BindAction(Button_C, ETriggerEvent::Completed, this, &ThisClass::CloseDoor_C);
	}
	if (IsValid(Button_D))
	{
		EIC->BindAction(Button_D, ETriggerEvent::Started, this, &ThisClass::OpenDoor_D);
		EIC->BindAction(Button_D, ETriggerEvent::Completed, this, &ThisClass::CloseDoor_D);
	}
}


void APS3ScreenPlayerController::OpenDoor_A()
{
	ServerRPC_OperateDoor(EControlDoorType::Door_A, true);
}

void APS3ScreenPlayerController::OpenDoor_B()
{
	ServerRPC_OperateDoor(EControlDoorType::Door_B, true);
}

void APS3ScreenPlayerController::OpenDoor_C()
{
	ServerRPC_OperateDoor(EControlDoorType::Door_C, true);
}

void APS3ScreenPlayerController::OpenDoor_D()
{
	ServerRPC_OperateDoor(EControlDoorType::Door_D, true);
}

void APS3ScreenPlayerController::CloseDoor_A()
{
	ServerRPC_OperateDoor(EControlDoorType::Door_A, false);
}

void APS3ScreenPlayerController::CloseDoor_B()
{
	ServerRPC_OperateDoor(EControlDoorType::Door_B, false);
}

void APS3ScreenPlayerController::CloseDoor_C()
{
	ServerRPC_OperateDoor(EControlDoorType::Door_C, false);
}

void APS3ScreenPlayerController::CloseDoor_D()
{
	ServerRPC_OperateDoor(EControlDoorType::Door_D, false);
}




//TODO 추가 사항 확인 필요: 배치 된 Door 액터 중 일치하는 이넘 값을 찾아서 해당 도어 Operating(구동) - 김명현
void APS3ScreenPlayerController::ServerRPC_OperateDoor_Implementation(EControlDoorType DoorType, bool bIsOpened)
{
	for (AControlDoor* ControllDoor : ControlDoorArray)
	{
		if (IsValid(ControllDoor) == false) continue;
		
		ControllDoor->NetMulti_OnOperateDoor(DoorType, bIsOpened);
	}
}


//TODO 추가 사항 확인 필요: 스크린플레이어용 인풋맵핑 추가 - 김명현
void APS3ScreenPlayerController::ConfigureLocalInputMode()
{
	if (bLocalInputModeConfigured == true || IsLocalController() == false) return;
	
	FInputModeGameAndUI InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;

	if (IsValid(InputMappingContext))
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			InputSubsystem->AddMappingContext(InputMappingContext, 0);
			bLocalInputModeConfigured = true;
		}
	}
		
}

void APS3ScreenPlayerController::SetCameraView()
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false) return;
	
	AActor* PS3CameraActor = UGameplayStatics::GetActorOfClass(World, APS3CameraActor::StaticClass());
	if (IsValid(PS3CameraActor) == false) return;
	
	SetViewTargetWithBlend(PS3CameraActor, 0.0f);
}


