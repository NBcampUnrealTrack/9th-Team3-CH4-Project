// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/Interaction/PS3InteractableInterface.h"
#include "FinalTreasureComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PUZZLESTAY3_API UFinalTreasureComponent : public UActorComponent, public IPS3InteractableInterface
{
	GENERATED_BODY()

public:
	UFinalTreasureComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool CanInteract_Implementation(AActor* Requestor) const override;
	virtual bool Interact_Implementation(AActor* Requestor) override;

protected:
	virtual void BeginPlay() override;
	
	// BP에서 Level Sequence 재생, UI 숨김, 입력 잠금 등을 처리
	UFUNCTION(BlueprintImplementableEvent, Category = "Final Treasure")
	void BP_PlayFinalCutscene();

};
