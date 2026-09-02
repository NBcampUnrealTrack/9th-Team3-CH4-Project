#pragma once

#include "CoreMinimal.h"
#include "Components/SlateWrapperTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PS3UIBlueprintFunctionLibrary.generated.h"

UCLASS()
class PUZZLESTAY3_API UPS3UIBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "UI|Conversion", meta = (DisplayName = "Bool To Visibility"))
	static ESlateVisibility BoolToVisibility(bool bValue);
};
