// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PS3ChoiceController.generated.h"

enum class EPS3PlayerRole : uint8;
/**
 * 
 */
UCLASS()
class PUZZLESTAY3_API APS3ChoiceController : public APlayerController
{
	GENERATED_BODY()
	
	APS3ChoiceController();
	
protected:
	virtual void BeginPlay() override;
	virtual void ReceivedPlayer() override;
	virtual void UpdateRotation(float DeltaTime) override;
	
	
public:
	//TODO 테스트용 BlueprintCallable 임. 나중에 삭제 할 것
	UFUNCTION(BlueprintCallable, Category = "Test")
	void OnClickedFieldTypeButton();
	UFUNCTION(BlueprintCallable, Category = "Test")
	void OnClickedScreenTypeButton();
	
	void ConfigureInputMapping();
	
public:
	UPROPERTY(EditAnywhere, Category = "UI")
	TArray<TSubclassOf<UUserWidget>> UIWidgetClass;
	
	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UUserWidget> UIWidgetInstance;

	UFUNCTION(Server, Reliable)
	void ServerRPC_SelectedControllerType(EPS3PlayerRole SelectedPlayerRoleType);
	
	UPROPERTY()
	TObjectPtr<class UCameraComponent> FixedCameraComponent;
	
protected:
	
	
	bool bIsSelectedFieldType = false;
	bool bIsSelectedScreenType = false;
	
};

