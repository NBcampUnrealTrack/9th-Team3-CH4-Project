// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3ChoiceController.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void APS3ChoiceController::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocalController() == false) return;
	
	if (UIWidgetClass.IsValidIndex(0) == false) return;
	
	if (UIWidgetClass[0] == nullptr) return;
	
	UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass[0]);
	if (IsValid(UIWidgetInstance) == false) return;
		
	UIWidgetInstance->AddToViewport();

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());
	SetInputMode(Mode);

	bShowMouseCursor = true;
	
}

void APS3ChoiceController::JoinServer(const FString& InIPAddress)
{
	FName NextLevelName = FName(*InIPAddress);
	UGameplayStatics::OpenLevel(GetWorld(), NextLevelName, true);
}