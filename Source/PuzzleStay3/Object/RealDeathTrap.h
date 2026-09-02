#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RealDeathTrap.generated.h"

class UShapeComponent;
class UStaticMeshComponent;

UCLASS()
class PUZZLESTAY3_API ARealDeathTrap : public AActor
{
	GENERATED_BODY()
	
public:	
	ARealDeathTrap();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UShapeComponent* CollisionComp;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
						bool bFromSweep, const FHitResult& SweepResult);
};
