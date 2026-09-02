#include "PS3UIBlueprintFunctionLibrary.h"

ESlateVisibility UPS3UIBlueprintFunctionLibrary::BoolToVisibility(bool bValue)
{
	return bValue ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
}
