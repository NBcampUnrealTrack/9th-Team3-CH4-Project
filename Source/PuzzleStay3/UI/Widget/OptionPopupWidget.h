#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionPopupWidget.generated.h"

UCLASS()
class PUZZLESTAY3_API UOptionPopupWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|OptionPopup")
	bool bIsOpen = false;
};
