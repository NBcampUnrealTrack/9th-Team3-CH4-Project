#include "PS3RootHUDWidget.h"

UTextNotifyWidget* UPS3RootHUDWidget::GetTextNotifyWidget() const
{
	return TextNotifyWidget;
}

ULifeCountWidget* UPS3RootHUDWidget::GetLifeCountWidget() const
{
	return LifeCountWidget;
}

UInteractionNotifyWidget* UPS3RootHUDWidget::GetInteractionNotifyWidget() const
{
	return InteractionNotifyWidget;
}

UTimerNotifyWidget* UPS3RootHUDWidget::GetTimerNotifyWidget() const
{
	return TimerNotifyWidget;
}

UTutorialNotifyWidget* UPS3RootHUDWidget::GetTutorialNotifyWidget() const
{
	return TutorialNotifyWidget;
}

UDoorOpenButtonWidget* UPS3RootHUDWidget::GetDoorOpenButtonWidget() const
{
	return DoorOpenButtonWidget;
}

UVoiceChatIconWidget* UPS3RootHUDWidget::GetVoiceChatIconWidget() const
{
	return VoiceChatIconWidget;
}
