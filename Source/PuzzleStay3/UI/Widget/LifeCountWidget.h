#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LifeCountWidget.generated.h"

UCLASS()
class PUZZLESTAY3_API ULifeCountWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|LifeCount")
	int32 CurrentRemainingLife = 0;
};
