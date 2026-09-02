#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionNotifyWidget.generated.h"

class UImage;
class UTexture2D;

UCLASS()
class PUZZLESTAY3_API UInteractionNotifyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void ShowInteractionNotify(FName InNotifyId, const FText& InKeyName);

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void HideInteractionNotify(FName InNotifyId);

	UFUNCTION(BlueprintCallable, Category = "UI|InteractionNotify")
	void HideAllInteractionNotifies();

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> InteractionImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|InteractionNotify")
	TObjectPtr<UTexture2D> FInteractionTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|InteractionNotify")
	TObjectPtr<UTexture2D> GInteractionTexture;

private:
	void RefreshInteractionImage();

	TMap<FName, FText> ActiveInteractionNotifies;
};
