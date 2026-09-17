// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "PlayerArrowComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PUZZLESTAY3_API UPlayerArrowComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
	
	UPlayerArrowComponent();
	
protected:
	virtual  void BeginPlay() override;
};
