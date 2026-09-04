#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/PlayerStartType.h"
#include "GameFramework/PlayerStart.h"
#include "PS3PlayerStartBase.generated.h"

enum class EPlayerStartType : uint8;

UCLASS()
class PUZZLESTAY3_API APS3PlayerStartBase : public APlayerStart
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(EditAnywhere, Category = "PlayerStartType")
	EPlayerStartType PlayerStartType = EPlayerStartType::None;
};
