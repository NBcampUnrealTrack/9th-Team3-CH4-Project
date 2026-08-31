#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PS3RootHUDWidget.generated.h"

class UTextNotifyWidget;

UCLASS()
class PUZZLESTAY3_API UPS3RootHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UTextNotifyWidget* GetTextNotifyWidget() const;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextNotifyWidget> TextNotifyWidget;
};
