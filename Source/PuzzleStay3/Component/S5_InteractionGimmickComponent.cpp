#include "S5_InteractionGimmickComponent.h"


US5_InteractionGimmickComponent::US5_InteractionGimmickComponent()
{

}

bool US5_InteractionGimmickComponent::CanInteract_Implementation(AActor* Requestor) const
{
	return IPS3InteractableInterface::CanInteract_Implementation(Requestor);
}

bool US5_InteractionGimmickComponent::Interact_Implementation(AActor* Requestor)
{
	return IPS3InteractableInterface::Interact_Implementation(Requestor);
	
}



