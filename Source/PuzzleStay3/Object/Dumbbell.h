#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dumbbell.generated.h"

UCLASS()
class PUZZLESTAY3_API ADumbbell : public AActor
{
	GENERATED_BODY()
	
public:	
	ADumbbell();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DumbbellMesh;

};
