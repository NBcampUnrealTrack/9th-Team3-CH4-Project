#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/Enum/PS3InteractionNotifyType.h"
#include "InteractionNotifyWidget.generated.h"

class UImage;
class UTexture2D;

UCLASS()
class PUZZLESTAY3_API UInteractionNotifyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void ShowInteractionNotifyWidget();

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void HideInteractionNotifyWidget();

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void ShowInteractionNotify(EPS3InteractionNotifyType NotifyType);

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void HideInteractionNotify(EPS3InteractionNotifyType NotifyType);

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void HideAllInteractionNotifies();

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void SetInteractionNotifyS5(EPS3InteractionNotifyType NotifyType, bool bVisible);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> InteractionImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|InteractionNotify")
	TObjectPtr<UTexture2D> FInteractionTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|InteractionNotify")
	TObjectPtr<UTexture2D> GInteractionTexture;

private:
	void RefreshInteractionImage();

	int32 InteractCount = 0;
	int32 DropCount = 0;
	bool bS5InteractVisible = false;
	bool bS5DropVisible = false;
};
