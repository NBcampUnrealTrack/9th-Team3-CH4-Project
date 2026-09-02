#include "TutorialNotifyWidget.h"

void UTutorialNotifyWidget::ShowTutorialNotify()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UTutorialNotifyWidget::HideTutorialNotify()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
