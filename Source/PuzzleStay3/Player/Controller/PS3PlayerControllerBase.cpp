// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3PlayerControllerBase.h"

#include "Core/GameMode/PS3GamemodeBase.h"
#include "Core/GameState/PS3GameStateS5.h"
#include "EnhancedInputComponent.h" // 현준 수정
#include "EnhancedInputSubsystems.h" // 현준 수정
#include "Engine/LocalPlayer.h" // 현준 수정
#include "InputAction.h" // 현준 수정
#include "InputMappingContext.h" // 현준 수정
#include "Kismet/GameplayStatics.h" // 현준 수정
#include "PS3ChoiceController.h" // 현준 수정
#include "PS3ScreenPlayerController.h" // 현준 수정
#include "Sound/SoundClass.h" // 현준 수정
#include "Sound/SoundMix.h" // 현준 수정
#include "UI/HUD/PlayerHUD.h"
#include "UI/ViewModel/PS3ViewModel.h"


void APS3PlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();
	//GetWorld()->GetTimerManager().SetTimer(InitTimerHandle, this, &ThisClass::ServerRPC_OnClickedRestartGameButton, 5.0f, false);
	
	GetWorld()->GetTimerManager().SetTimer(InitTimerHandle, this, &ThisClass::ConfigureInputMapping, 0.01f, false);
}

void APS3PlayerControllerBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveOptionMappingContext(); // 현준 수정

	if (PS3ViewModel == nullptr) return;
	PS3ViewModel->OnGameRestartRequested_UI.RemoveDynamic(this, &ThisClass::OnClickedRestartGameButton);
	PS3ViewModel->OnExitToMainRequested_UI.RemoveDynamic(this, &ThisClass::OnClickedTitleMenuButton);
	PS3ViewModel->OnBGMVolumeChanged_UI.RemoveDynamic(this, &ThisClass::HandleBGMVolumeChanged); // 현준 수정
	PS3ViewModel->OnSFXVolumeChanged_UI.RemoveDynamic(this, &ThisClass::HandleSFXVolumeChanged); // 현준 수정
	
	Super::EndPlay(EndPlayReason);
}


void APS3PlayerControllerBase::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	ConfigureInputMapping();
	//GetWorld()->GetTimerManager().SetTimer(InitTimerHandle, this, &ThisClass::ConfigureInputMapping, 0.01f, false);
}

void APS3PlayerControllerBase::SetupInputComponent() // 현준 수정
{ // 현준 수정
	Super::SetupInputComponent(); // 현준 수정
 // 현준 수정
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent); // 현준 수정
	if (IsValid(EnhancedInputComponent) == false || IsValid(OptionAction) == false) return; // 현준 수정
 // 현준 수정
	EnhancedInputComponent->BindAction(OptionAction, ETriggerEvent::Started, this, &ThisClass::HandleOptionStarted); // 현준 수정
} // 현준 수정


void APS3PlayerControllerBase::ConfigureInputMapping()
{
	if (IsLocalController() == false) return;
	
	GetWorld()->GetTimerManager().ClearTimer(InitTimerHandle);
	AddOptionMappingContext(); // 현준 수정
	
	/*
	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = true;*/
	
	auto* HUD = Cast<APlayerHUD>(GetHUD());
	if (HUD == nullptr) return;
	
	ConfigureViewModelBindings(HUD->GetViewModel());
}


void APS3PlayerControllerBase::ConfigureViewModelBindings(UPS3ViewModel* InViewModel)
{
	if (IsLocalController() == false) return;
	
	if (IsValid(InViewModel) == false) return;

	if (IsValid(PS3ViewModel) && (PS3ViewModel.Get() == InViewModel) == false)
	{
		PS3ViewModel->OnGameRestartRequested_UI.RemoveDynamic(this,&ThisClass::OnClickedRestartGameButton);
		PS3ViewModel->OnExitToMainRequested_UI.RemoveDynamic(this,&ThisClass::OnClickedTitleMenuButton);
		PS3ViewModel->OnBGMVolumeChanged_UI.RemoveDynamic(this, &ThisClass::HandleBGMVolumeChanged); // 현준 수정
		PS3ViewModel->OnSFXVolumeChanged_UI.RemoveDynamic(this, &ThisClass::HandleSFXVolumeChanged); // 현준 수정
	}
	
	PS3ViewModel = InViewModel;

	PS3ViewModel->OnGameRestartRequested_UI.AddUniqueDynamic(this,&ThisClass::OnClickedRestartGameButton);
	PS3ViewModel->OnExitToMainRequested_UI.AddUniqueDynamic(this,&ThisClass::OnClickedTitleMenuButton);
	PS3ViewModel->OnBGMVolumeChanged_UI.AddUniqueDynamic(this, &ThisClass::HandleBGMVolumeChanged); // 현준 수정
	PS3ViewModel->OnSFXVolumeChanged_UI.AddUniqueDynamic(this, &ThisClass::HandleSFXVolumeChanged); // 현준 수정
	PushConfiguredSoundMix(); // 현준 수정

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(InitTimerHandle);
	}
}

void APS3PlayerControllerBase::AddOptionMappingContext() // 현준 수정
{ // 현준 수정
	if (bOptionInputConfigured || IsLocalController() == false || IsValid(OptionMappingContext) == false) return; // 현준 수정
 // 현준 수정
	if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) // 현준 수정
	{ // 현준 수정
		InputSubsystem->AddMappingContext(OptionMappingContext, 10); // 현준 수정
		bOptionInputConfigured = true; // 현준 수정
	} // 현준 수정
} // 현준 수정

void APS3PlayerControllerBase::RemoveOptionMappingContext() // 현준 수정
{ // 현준 수정
	if (bOptionInputConfigured == false || IsLocalController() == false || IsValid(OptionMappingContext) == false) return; // 현준 수정
 // 현준 수정
	if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) // 현준 수정
	{ // 현준 수정
		InputSubsystem->RemoveMappingContext(OptionMappingContext); // 현준 수정
	} // 현준 수정
 // 현준 수정
	bOptionInputConfigured = false; // 현준 수정
} // 현준 수정

void APS3PlayerControllerBase::HandleOptionStarted() // 현준 수정
{ // 현준 수정
	if (IsLocalController() == false || GetNetMode() == NM_DedicatedServer) return; // 현준 수정
 // 현준 수정
	if (IsValid(PS3ViewModel)) // 현준 수정
	{ // 현준 수정
		PS3ViewModel->RequestToggleOptionPopup(); // 현준 수정
	} // 현준 수정
	else if (APlayerHUD* PlayerHUD = Cast<APlayerHUD>(GetHUD())) // 현준 수정
	{ // 현준 수정
		PlayerHUD->ToggleOptionPopup(); // 현준 수정
	} // 현준 수정
	else // 현준 수정
	{ // 현준 수정
		return; // 현준 수정
	} // 현준 수정
 // 현준 수정
	bIsOptionPopupOpenByInput = !bIsOptionPopupOpenByInput; // 현준 수정
	ApplyOptionPopupInputMode(bIsOptionPopupOpenByInput); // 현준 수정
} // 현준 수정

void APS3PlayerControllerBase::ApplyOptionPopupInputMode(bool bOpen) // 현준 수정
{ // 현준 수정
	if (IsLocalController() == false) return; // 현준 수정
 // 현준 수정
	if (bOpen) // 현준 수정
	{ // 현준 수정
		FInputModeGameAndUI InputMode; // 현준 수정
		InputMode.SetHideCursorDuringCapture(false); // 현준 수정
		SetInputMode(InputMode); // 현준 수정
		bShowMouseCursor = true; // 현준 수정
		return; // 현준 수정
	} // 현준 수정
 // 현준 수정
	if (IsA<APS3ChoiceController>()) // 현준 수정
	{ // 현준 수정
		FInputModeUIOnly InputMode; // 현준 수정
		SetInputMode(InputMode); // 현준 수정
		bShowMouseCursor = true; // 현준 수정
		return; // 현준 수정
	} // 현준 수정
 // 현준 수정
	if (IsA<APS3ScreenPlayerController>()) // 현준 수정
	{ // 현준 수정
		FInputModeGameAndUI InputMode; // 현준 수정
		SetInputMode(InputMode); // 현준 수정
		bShowMouseCursor = false; // 현준 수정
		return; // 현준 수정
	} // 현준 수정
 // 현준 수정
	FInputModeGameOnly InputMode; // 현준 수정
	SetInputMode(InputMode); // 현준 수정
	bShowMouseCursor = false; // 현준 수정
} // 현준 수정

void APS3PlayerControllerBase::HandleBGMVolumeChanged(float Value) // 현준 수정
{ // 현준 수정
	ApplySoundClassVolume(MasterSoundClass, Value, true); // 현준 수정
} // 현준 수정

void APS3PlayerControllerBase::HandleSFXVolumeChanged(float Value) // 현준 수정
{ // 현준 수정
	ApplySoundClassVolume(SFXSoundClass, Value, false); // 현준 수정
} // 현준 수정

void APS3PlayerControllerBase::PushConfiguredSoundMix() // 현준 수정
{ // 현준 수정
	if (IsLocalController() == false || GetNetMode() == NM_DedicatedServer) return; // 현준 수정
	if (IsValid(MasterSoundMix) == false) return; // 현준 수정
	UGameplayStatics::PushSoundMixModifier(this, MasterSoundMix); // 현준 수정
} // 현준 수정

void APS3PlayerControllerBase::ApplySoundClassVolume(USoundClass* SoundClass, float Value, bool bApplyToChildren) // 현준 수정
{ // 현준 수정
	if (IsLocalController() == false || GetNetMode() == NM_DedicatedServer) return; // 현준 수정
	if (IsValid(MasterSoundMix) == false || IsValid(SoundClass) == false) return; // 현준 수정
	PushConfiguredSoundMix(); // 현준 수정
	UGameplayStatics::SetSoundMixClassOverride( // 현준 수정
		this, // 현준 수정
		MasterSoundMix, // 현준 수정
		SoundClass, // 현준 수정
		FMath::Clamp(Value, 0.0f, 1.0f), // 현준 수정
		1.0f, // 현준 수정
		0.0f, // 현준 수정
		bApplyToChildren); // 현준 수정
} // 현준 수정


void APS3PlayerControllerBase::OnClickedRestartGameButton()
{
	UE_LOG(LogTemp, Error, TEXT("플레이어컨트롤러베이스 들어옴!!!!!!"));
	ServerRPC_OnClickedRestartGameButton();
}


void APS3PlayerControllerBase::OnClickedTitleMenuButton()
{
	ServerRPC_OnClickedTitleButton();
}


void APS3PlayerControllerBase::ServerRPC_OnClickedRestartGameButton_Implementation()
{
	auto* PS3GameModeBase = Cast<APS3GameModeBase>(GetWorld()->GetAuthGameMode());
	if (IsValid(PS3GameModeBase) == false) return;
	
	PS3GameModeBase->StageRestart();
}


void APS3PlayerControllerBase::ServerRPC_OnClickedTitleButton_Implementation()
{
	auto* PS3GameModeBase = Cast<APS3GameModeBase>(GetWorld()->GetAuthGameMode());
	if (IsValid(PS3GameModeBase) == false) return;
	
	PS3GameModeBase->GotoTitleLevel();
}






