// Fill out your copyright notice in the Description page of Project Settings.


#include "FinalTreasureComponent.h"

#include "Core/GameMode/PS3GameModeEnd.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"

UFinalTreasureComponent::UFinalTreasureComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UFinalTreasureComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool UFinalTreasureComponent::CanInteract_Implementation(AActor* Requestor) const
{
	const APawn* Pawn = Cast<APawn>(Requestor);
	if (!IsValid(Pawn)){return false;}

	APlayerState* PS = Pawn->GetPlayerState();
	if (!IsValid(PS)){return false;}

	const UWorld* World = GetWorld();
	if (!IsValid(World)){return false;}

	APS3GameModeEnd* GM = World->GetAuthGameMode<APS3GameModeEnd>();
	if (!IsValid(GM)){return false;}

	return GM->CanInteractFinalTreasure(PS);
}

bool UFinalTreasureComponent::Interact_Implementation(AActor* Requestor)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner) || !Owner->HasAuthority()){return false;}

	const APawn* Pawn = Cast<APawn>(Requestor);
	if (!IsValid(Pawn)){return false;}

	APlayerState* PS = Pawn->GetPlayerState();
	if (!IsValid(PS)){return false;}

	APS3GameModeEnd* GM = GetWorld()->GetAuthGameMode<APS3GameModeEnd>();
	if (!IsValid(GM)){return false;}

	return GM->RegisterFinalTreasureInteraction(PS);
}
