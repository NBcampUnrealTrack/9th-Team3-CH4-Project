// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PS3ChoiceController.generated.h"

enum class EPS3PlayerRoleType : uint8;
/**
 * 
 */
UCLASS()
class PUZZLESTAY3_API APS3ChoiceController : public APlayerController
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	

public:
	UPROPERTY(EditAnywhere, Category = "UI")
	TArray<TSubclassOf<UUserWidget>> UIWidgetClass;
	
	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UUserWidget> UIWidgetInstance;

	UFUNCTION(Server, Reliable)
	void ServerRPC_SelectedControllerType(EPS3PlayerRoleType SelectedPlayerRoleType);
	
	UFUNCTION(BlueprintCallable, Category = "Test")
	void OnClickedThirdPersonTypeButton();
	
	UFUNCTION(BlueprintCallable, Category = "Test")
	void OnClickedScreenTypeButton();
};

