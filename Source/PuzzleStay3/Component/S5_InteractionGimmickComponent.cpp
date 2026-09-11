#include "S5_InteractionGimmickComponent.h"

#include "Core/GameMode/PS3GameModeS5.h"
#include "Core/GameState/PS3GameStateS5.h"
<<<<<<< HEAD
#include "Data/Delegates/S5_GameRuleDelegateComponent.h"
=======
>>>>>>> dev
#include "Net/UnrealNetwork.h"


US5_InteractionGimmickComponent::US5_InteractionGimmickComponent()
{

}

void US5_InteractionGimmickComponent::BeginPlay()
{
	Super::BeginPlay();
<<<<<<< HEAD
	
	PS3_S5_GAME_RULE_DELEGATE_BINDING_FUNCTION(OnIsInteractionGimmick, OnColletedGimmickBase);
	PS3_S5_GAME_RULE_DELEGATE_BINDING_FUNCTION(OnIsGameStart, OnStartedGame);
=======
	auto* PS3GameModeS5 = Cast<APS3GameModeS5>(GetWorld()->GetAuthGameMode());
	if (IsValid(PS3GameModeS5) == false) return;
	PS3GameModeS5->OnIsInteractionGimmick.AddUObject(this, &ThisClass::OnColletedGimmickBase);
	PS3GameModeS5->OnIsGameStart.AddUObject(this, &ThisClass::OnStartedGame);
>>>>>>> dev
}


void US5_InteractionGimmickComponent::GetLifetimeReplicatedProps(
	TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, bIsInteractionGimmick);
	DOREPLIFETIME(ThisClass, bIsInteractedGimmick);
	DOREPLIFETIME(ThisClass, bIsStartedGame);
}

bool US5_InteractionGimmickComponent::CanInteract_Implementation(AActor* Requestor) const
{
	if (bIsStartedGame == false) return false;
	
	return true;
}

void US5_InteractionGimmickComponent::OnStartedGame(bool bIsGameStart)
{
	bIsStartedGame = bIsGameStart;
}

bool US5_InteractionGimmickComponent::Interact_Implementation(AActor* Requestor)
{
	if (bIsInteractionGimmick == false) return false;
	
	if (bIsInteractedGimmick == true) return false;
	
<<<<<<< HEAD
	PS3_S5_GAME_RULE_DELEGATE_BROADCAST_OneParams(OnInteractedGimmick, bIsInteractedGimmick);
=======
	OnInteractionGimmick.Broadcast(bIsInteractedGimmick);
>>>>>>> dev
	bIsInteractedGimmick = true;
	
	
	return true;
}




void US5_InteractionGimmickComponent::OnColletedGimmickBase(const UActorComponent* CurrentComponent, bool bIsInteractable)
{
	if (this == CurrentComponent)
	{
		bIsInteractionGimmick = bIsInteractable;
	}
}










