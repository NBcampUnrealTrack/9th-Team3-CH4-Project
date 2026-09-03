// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PS3GamemodeBase.h"
#include "PS3GameModeEnd.generated.h"


UCLASS()
class PUZZLESTAY3_API APS3GameModeEnd : public APS3GameModeBase
{
	GENERATED_BODY()
	
	UFUNCTION()
	virtual void HandleSwitchActivatedChanged(bool bActivated) override;
};
