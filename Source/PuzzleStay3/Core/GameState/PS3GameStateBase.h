// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PS3GameStateBase.generated.h"
//DECLARE_MULTICAST_DELEGATE_OneParam(FOnEscapeDoorOpened, EDoorType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEscapeDoorOpened, bool, bOpened);

UCLASS()
class PUZZLESTAY3_API APS3GameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	APS3GameStateBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure)
	bool IsEscapeDoorOpened() const { return bEscapeDoorOpened; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetEscapeDoorOpened(bool bOpened);

	UPROPERTY(BlueprintAssignable)
	FOnEscapeDoorOpened OnEscapeDoorOpened;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_EscapeDoorOpened)
	bool bEscapeDoorOpened = false;

	UFUNCTION()
	void OnRep_EscapeDoorOpened();
	
	
	void SetUIMacroTimerHandle(TFunction<void()> Callback);
	
	
};