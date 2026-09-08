#include "PS3ScreenPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Data/Enum/ControlDoorType.h"
#include "Data/Enum/DoorType.h"
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
		EIC->BindAction(Button_A, ETriggerEvent::Started, this, &ThisClass::OpenDoor);
		EIC->BindAction(Button_A, ETriggerEvent::Completed, this, &ThisClass::CloseDoor);
	}
	if (IsValid(Button_B))
	{
		EIC->BindAction(Button_B, ETriggerEvent::Started, this, &ThisClass::OpenDoor);
		EIC->BindAction(Button_B, ETriggerEvent::Completed, this, &ThisClass::CloseDoor);
	}
	if (IsValid(Button_C))
	{
		EIC->BindAction(Button_C, ETriggerEvent::Started, this, &ThisClass::OpenDoor);
		EIC->BindAction(Button_C, ETriggerEvent::Completed, this, &ThisClass::CloseDoor);
	}
	if (IsValid(Button_D))
	{
		EIC->BindAction(Button_D, ETriggerEvent::Started, this, &ThisClass::OpenDoor);
		EIC->BindAction(Button_D, ETriggerEvent::Completed, this, &ThisClass::CloseDoor);
	}
}


void APS3ScreenPlayerController::OpenDoor(const FInputActionInstance& Instance)
{
	EControlDoorType PressedDoorType = GetDoorTypeFromAction(Instance.GetSourceAction());
	if (PressedDoorType == EControlDoorType::None) return;
	
	if (CurrentOpenedDoorType == EControlDoorType::None)
	{
		CurrentOpenedDoorType = PressedDoorType;
		ServerRPC_OperateDoor(PressedDoorType, true);
	}
	
}


void APS3ScreenPlayerController::CloseDoor(const FInputActionInstance& Instance)
{
	EControlDoorType PressedDoorType = GetDoorTypeFromAction(Instance.GetSourceAction());
	if (PressedDoorType == EControlDoorType::None) return;

	if (CurrentOpenedDoorType == PressedDoorType)
	{
		CurrentOpenedDoorType = EControlDoorType::None;
		ServerRPC_OperateDoor(PressedDoorType, false);
	}
}


EControlDoorType APS3ScreenPlayerController::GetDoorTypeFromAction(const UInputAction* Action) const
{
	if (Action == Button_A) return EControlDoorType::Door_A;
	if (Action == Button_B) return EControlDoorType::Door_B;
	if (Action == Button_C) return EControlDoorType::Door_C;
	if (Action == Button_D) return EControlDoorType::Door_D;

	return EControlDoorType::None;
}


void APS3ScreenPlayerController::ServerRPC_OperateDoor_Implementation(EControlDoorType DoorType, bool bIsOpen)
{
	for (AControlDoor* ControlDoor : ControlDoorArray)
	{
		if (IsValid(ControlDoor) == false) continue;
		
		ControlDoor->NetMulti_OnOperateDoor(DoorType, bIsOpen);
	}
}


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


