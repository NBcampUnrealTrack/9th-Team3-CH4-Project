#include "S5_InteractionGimmickComponent.h"

#include "Net/UnrealNetwork.h"


US5_InteractionGimmickComponent::US5_InteractionGimmickComponent()
{

}


void US5_InteractionGimmickComponent::GetLifetimeReplicatedProps(
	TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, bIsInteractionGimmick);
	DOREPLIFETIME(ThisClass, bIsInteractedGimmick);
}


bool US5_InteractionGimmickComponent::CanInteract_Implementation(AActor* Requestor) const
{
	return false;
}


bool US5_InteractionGimmickComponent::Interact_Implementation(AActor* Requestor)
{
	if (bIsInteractionGimmick == false) return false;
	if (!CanInteract_Implementation(Requestor))	return false;
	
	OnInteractedGimmick.Broadcast(bIsInteractedGimmick);
	
	return false;
}





