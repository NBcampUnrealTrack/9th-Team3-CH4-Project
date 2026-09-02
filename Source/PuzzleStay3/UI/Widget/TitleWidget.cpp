#include "TitleWidget.h"

void UTitleWidget::ShowTitle()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UTitleWidget::HideTitle()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
