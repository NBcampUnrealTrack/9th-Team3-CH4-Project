#include "GameOverWidget.h"

void UGameOverWidget::ShowGameOver()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UGameOverWidget::HideGameOver()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
