#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LifeCountWidget.generated.h"

class UHorizontalBox;
class UImage;
class UTexture2D;
class UPS3ViewModel;

UCLASS()
class PUZZLESTAY3_API ULifeCountWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|ViewModel")
	void SetViewModel(UPS3ViewModel* InViewModel);

	UFUNCTION(BlueprintCallable, Category = "UI|LifeCount")
	void ShowLifeCount();

	UFUNCTION(BlueprintCallable, Category = "UI|LifeCount")
	void HideLifeCount();

	UFUNCTION(BlueprintCallable, Category = "UI|LifeCount")
	void UpdateLifeCount(int32 InCurrentLifeCount, int32 InMaxLifeCount);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UHorizontalBox> HeartContainer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|LifeCount")
	TObjectPtr<UTexture2D> FilledHeartTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|LifeCount")
	TObjectPtr<UTexture2D> EmptyHeartTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|LifeCount")
	float HeartSize = 32.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|LifeCount")
	float HeartSpacing = 4.0f;

	UPROPERTY(BlueprintReadOnly, Category = "UI|ViewModel")
	TObjectPtr<UPS3ViewModel> ViewModel;

private:
	UPROPERTY()
	TArray<TObjectPtr<UImage>> HeartImages;

	void RebuildHearts(int32 MaxLife);
	void RefreshHearts(int32 CurrentLife);
};
