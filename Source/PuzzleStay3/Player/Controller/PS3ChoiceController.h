// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PS3ChoiceController.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLESTAY3_API APS3ChoiceController : public APlayerController
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	
	void JoinServer(const FString& InIPAddress);

public:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> UIWidgetClass;
	
	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UUserWidget> UIWidgetInstance;
	
};
