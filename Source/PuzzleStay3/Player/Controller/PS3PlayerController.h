#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PS3PlayerController.generated.h"

UCLASS()
class PUZZLESTAY3_API APS3PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APS3PlayerController();

	UFUNCTION(BlueprintPure, Category = "PS3|Player Controller")
	bool IsFieldPlayer() const;

protected:
	virtual void BeginPlay() override;
	virtual void OnRep_PlayerState() override;

private:
	void RefreshPlayerHUD() const;
};
