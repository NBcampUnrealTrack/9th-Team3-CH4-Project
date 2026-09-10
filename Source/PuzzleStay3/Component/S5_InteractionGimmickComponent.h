#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/Interaction/PS3InteractableInterface.h"
#include "S5_InteractionGimmickComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnInteractedGimmick, bool);
DECLARE_MULTICAST_DELEGATE(FOnCosmeticInteractionSuccessed);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PUZZLESTAY3_API US5_InteractionGimmickComponent : public UActorComponent , public IPS3InteractableInterface
{
	GENERATED_BODY()

public:
	US5_InteractionGimmickComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual bool CanInteract_Implementation(AActor* Requestor) const override;
	virtual bool Interact_Implementation(AActor* Requestor) override;
	
	UPROPERTY(Replicated)
	bool bIsEscapeDoor = false;
	UPROPERTY(Replicated)
	bool bIsInteractedGimmick = false;
	
	FOnInteractedGimmick OnInteractedGimmick;
};