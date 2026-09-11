#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/Interaction/PS3InteractableInterface.h"
#include "S5_InteractionGimmickComponent.generated.h"

<<<<<<< HEAD
=======
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInteractionGimmick, bool);
>>>>>>> dev

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PUZZLESTAY3_API US5_InteractionGimmickComponent : public UActorComponent , public IPS3InteractableInterface
{
	GENERATED_BODY()

public:
	US5_InteractionGimmickComponent();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual bool CanInteract_Implementation(AActor* Requestor) const override;
	virtual bool Interact_Implementation(AActor* Requestor) override;
	
	void OnStartedGame(bool bIsGameStart);
	void OnColletedGimmickBase(const UActorComponent* CurrentComponent, bool bIsInteractable);
	
<<<<<<< HEAD
=======
	FOnInteractionGimmick OnInteractionGimmick;
>>>>>>> dev
	
	UPROPERTY(Replicated)
	bool bIsInteractionGimmick = false;
	
	UPROPERTY(Replicated)
	bool bIsInteractedGimmick = false;
	
	UPROPERTY(Replicated)
	bool bIsStartedGame = false;
	
	
};



