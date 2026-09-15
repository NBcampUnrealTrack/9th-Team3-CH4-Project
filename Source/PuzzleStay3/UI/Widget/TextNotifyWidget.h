#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/Enum/PS3TextNotifyType.h"
#include "TimerManager.h"
#include "TextNotifyWidget.generated.h"

class UTextBlock;
class USoundBase; // 현준 수정

UCLASS()
class PUZZLESTAY3_API UTextNotifyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|TextNotify")
	void ShowTextNotify(EPS3TextNotifyType NotifyType);

	UFUNCTION(BlueprintCallable, Category = "UI|TextNotify")
	void SetTextNotifyVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "UI|TextNotify")
	void HideTextNotify();

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TextNotifyText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI|TextNotify|Sound") // 현준 수정
	TObjectPtr<USoundBase> TextNotifySound; // 현준 수정

private:
	void ShowTextNotifyInternal(const FText& InDisplayText, float InFontSize, float InDisplayDuration);

	FTimerHandle TextNotifyTimerHandle;
};
