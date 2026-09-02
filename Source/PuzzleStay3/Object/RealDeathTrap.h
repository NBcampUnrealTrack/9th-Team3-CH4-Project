#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RealDeathTrap.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnRealDeathTrapTriggered,
	APawn*,
	TriggeringPawn);

UCLASS()
class PUZZLESTAY3_API ARealDeathTrap : public AActor
{
	GENERATED_BODY()
	
public:	
	ARealDeathTrap();

	UPROPERTY(BlueprintAssignable, Category = "Real Death Trap")
	FOnRealDeathTrapTriggered OnRealDeathTrapTriggered;
	
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
	
private:
	
	bool bTriggered = false;
};
