#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RandomCollisionTrap.generated.h"

UCLASS()
class PUZZLESTAY3_API ARandomCollisionTrap : public AActor
{
	GENERATED_BODY()
	
public:	
	ARandomCollisionTrap();
	
	void SetPlatformCollision(bool bEnableCollision);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> PlatformMesh;
	
	UPROPERTY(ReplicatedUsing = OnRep_HasPlatformCollision)
	bool bHasPlatformCollision = false;

	UFUNCTION()
	void OnRep_HasPlatformCollision();

	void ApplyCollisionState();

public:
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
