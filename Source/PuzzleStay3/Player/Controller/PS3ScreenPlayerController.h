#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PS3ScreenPlayerController.generated.h"

UCLASS()
class PUZZLESTAY3_API APS3ScreenPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APS3ScreenPlayerController();

	UFUNCTION(BlueprintPure, Category = "PS3|Screen Player Controller")
	bool IsScreenPlayer() const;

protected:
	virtual void BeginPlay() override;
};
