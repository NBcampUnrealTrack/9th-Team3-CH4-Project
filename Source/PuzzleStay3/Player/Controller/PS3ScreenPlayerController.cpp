#include "PS3ScreenPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Data/Delegates/UIDelegatesSubsystem.h"
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


void APS3ScreenPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//ShowToCeiling();
	
	Super::EndPlay(EndPlayReason);
}


void APS3ScreenPlayerController::ConfigureViewModelBindings(UPS3ViewModel* InViewModel)
{
	Super::ConfigureViewModelBindings(InViewModel);
		
	if (IsLocalPlayerController() == true)
	{
		OnScreenPlayerUI_Show();
	}
	
}


void APS3ScreenPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	
	checkf(IsValid(InputMappingContext) == true, TEXT("스크린 컨트롤러 IMC 할당 안됨"));
	
	ContainDoorArray();

}


void APS3ScreenPlayerController::OnScreenPlayerUI_Show() const
{	
	if (IsLocalPlayerController() == true)
	{
		PS3_UIDELEGATE_TIMER_FOR_MACRO(PS3_BROADCAST_TO_MVVM_OneParams(OnScreenPlayer_UI, true));
	}
}




void APS3ScreenPlayerController::ContainDoorArray()
{
	if (HasAuthority() == false) return;
	
	TArray<AActor*> ActorArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AControlDoor::StaticClass(),ActorArray);
	
	for (AActor* Actor : ActorArray)
	{
		auto* ControlDoor = Cast<AControlDoor>(Actor);
		if (IsValid(ControlDoor) == false) continue;
		
		ControlDoorArray.Add(ControlDoor);
	}
}


void APS3ScreenPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (IsValid(EIC) == false) return;
	
	if (DeBugType == EDeBugType::OriginController)
	{
		checkf(IsValid(Button_A) == true,TEXT("스크린 컨트롤러 버튼A 할당 안됨"));
		{
			EIC->BindAction(Button_A, ETriggerEvent::Started, this, &ThisClass::OpenDoor);
			EIC->BindAction(Button_A, ETriggerEvent::Completed, this, &ThisClass::CloseDoor);
		}
		checkf(IsValid(Button_B) == true,TEXT("스크린 컨트롤러 버튼B 할당 안됨"));
		{
			EIC->BindAction(Button_B, ETriggerEvent::Started, this, &ThisClass::OpenDoor);
			EIC->BindAction(Button_B, ETriggerEvent::Completed, this, &ThisClass::CloseDoor);
		}
		checkf(IsValid(Button_C) == true,TEXT("스크린 컨트롤러 버튼C 할당 안됨"));
		{
			EIC->BindAction(Button_C, ETriggerEvent::Started, this, &ThisClass::OpenDoor);
			EIC->BindAction(Button_C, ETriggerEvent::Completed, this, &ThisClass::CloseDoor);
		}
		checkf(IsValid(Button_D) == true,TEXT("스크린 컨트롤러 버튼D 할당 안됨"));
		{
			EIC->BindAction(Button_D, ETriggerEvent::Started, this, &ThisClass::OpenDoor);
			EIC->BindAction(Button_D, ETriggerEvent::Completed, this, &ThisClass::CloseDoor);
		}
	}
	
	if (DeBugType == EDeBugType::TestController)
	{
		checkf(IsValid(Button_A) == true,TEXT("스크린 컨트롤러 버튼A 할당 안됨"));
		{
			EIC->BindAction(Button_A, ETriggerEvent::Started, this, &ThisClass::TestToggledDoor);
		}
		checkf(IsValid(Button_B) == true,TEXT("스크린 컨트롤러 버튼B 할당 안됨"));
		{
			EIC->BindAction(Button_B, ETriggerEvent::Started, this, &ThisClass::TestToggledDoor);
		}
		checkf(IsValid(Button_C) == true,TEXT("스크린 컨트롤러 버튼C 할당 안됨"));
		{
			EIC->BindAction(Button_C, ETriggerEvent::Started, this, &ThisClass::TestToggledDoor);
		}
		checkf(IsValid(Button_D) == true,TEXT("스크린 컨트롤러 버튼D 할당 안됨"));
		{
			EIC->BindAction(Button_D, ETriggerEvent::Started, this, &ThisClass::TestToggledDoor);
		}
	}
	
}


void APS3ScreenPlayerController::TestToggledDoor(const FInputActionInstance& Instance)
{
	EControlDoorType PressedDoorType = GetDoorTypeFromAction(Instance.GetSourceAction());
	if (PressedDoorType == EControlDoorType::None) return;

	// 닫는 로직(CloseDoor 및 이전 문 닫기)을 모두 제거하고 열기만 수행
	CurrentOpenedDoorType = PressedDoorType;
	PS3_BROADCAST_TO_MVVM_TwoParams(OnButtonEnabled_UI, PressedDoorType, true);
	ServerRPC_OperateDoor(PressedDoorType, true);
}


void APS3ScreenPlayerController::OpenDoor(const FInputActionInstance& Instance)
{
	EControlDoorType PressedDoorType = GetDoorTypeFromAction(Instance.GetSourceAction());
	if (PressedDoorType == EControlDoorType::None) return;
	
	if (CurrentOpenedDoorType == EControlDoorType::None)
	{
		CurrentOpenedDoorType = PressedDoorType;
		PS3_BROADCAST_TO_MVVM_TwoParams(OnButtonEnabled_UI, PressedDoorType, true);
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
		
		PS3_BROADCAST_TO_MVVM_TwoParams(OnButtonEnabled_UI, CurrentOpenedDoorType, false);
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
	
	//HideToCeiling();
}


void APS3ScreenPlayerController::ShowToCeiling()
{	
	if (IsLocalPlayerController() == true)
	{	
		TArray<AActor*> CeilingActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Ceiling"), CeilingActors);
		
		for (AActor* Actor : CeilingActors)
		{
			if (IsValid(Actor) == false) continue;
			
			UStaticMeshComponent* MeshComp = Actor->FindComponentByClass<UStaticMeshComponent>();
			if (IsValid(MeshComp) == false) continue;
			
			MeshComp->SetVisibility(true);
			MeshComp->SetCastHiddenShadow(false);	
		}
	}
}


void APS3ScreenPlayerController::HideToCeiling()
{
	if (IsLocalController() == true)
	{
		TArray<AActor*> CeilingActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Ceiling"), CeilingActors);

		checkf(CeilingActors.Num() > 0, TEXT("천장 메쉬 ActorTag 배열에 [Ceiling] Tag를 추가해주세요."));
		
		for (AActor* Actor : CeilingActors)
		{
			if (IsValid(Actor) == false) continue;
		
			UStaticMeshComponent* MeshComp = Actor->FindComponentByClass<UStaticMeshComponent>();
			if (IsValid(MeshComp) == false) continue;
		
			MeshComp->SetVisibility(false);
			MeshComp->SetCastHiddenShadow(true);	
		}	
		
	}
}




