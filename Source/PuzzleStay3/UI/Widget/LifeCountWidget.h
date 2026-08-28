#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LifeCountWidget.generated.h"

class UPS3ViewModel;

UCLASS()
class PUZZLESTAY3_API ULifeCountWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetViewModel(UPS3ViewModel* InViewModel);

	UFUNCTION(BlueprintCallable, Category = "UI|LifeCount")
	void UpdateLifeCount(int32 InCurrentLifeCount, int32 InMaxLifeCount);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "UI|ViewModel")
	TObjectPtr<UPS3ViewModel> ViewModel;
};
