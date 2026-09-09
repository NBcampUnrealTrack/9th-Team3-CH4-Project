#include "InteractionNotifyWidget.h"

#include "Components/Image.h"

void UInteractionNotifyWidget::ShowInteractionNotifyWidget()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UInteractionNotifyWidget::HideInteractionNotifyWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UInteractionNotifyWidget::ShowInteractionNotify(EPS3InteractionNotifyType NotifyType)
{
	switch (NotifyType)
	{
	case EPS3InteractionNotifyType::Interact:
		++InteractCount;
		break;
	case EPS3InteractionNotifyType::Drop:
		++DropCount;
		break;
	default:
		break;
	}

	RefreshInteractionImage();
}

void UInteractionNotifyWidget::HideInteractionNotify(EPS3InteractionNotifyType NotifyType)
{
	switch (NotifyType)
	{
	case EPS3InteractionNotifyType::Interact:
		InteractCount = FMath::Max(InteractCount - 1, 0);
		break;
	case EPS3InteractionNotifyType::Drop:
		DropCount = FMath::Max(DropCount - 1, 0);
		break;
	default:
		break;
	}

	RefreshInteractionImage();
}

void UInteractionNotifyWidget::HideAllInteractionNotifies()
{
	InteractCount = 0;
	DropCount = 0;
	RefreshInteractionImage();
}

void UInteractionNotifyWidget::RefreshInteractionImage()
{
	if (!InteractionImage)
	{
		return;
	}

	UTexture2D* TextureToShow = DropCount > 0 ? GInteractionTexture : nullptr;
	if (!TextureToShow && InteractCount > 0)
	{
		TextureToShow = FInteractionTexture;
	}

	if (!TextureToShow)
	{
		InteractionImage->SetVisibility(ESlateVisibility::Collapsed);
		HideInteractionNotifyWidget();
		return;
	}

	InteractionImage->SetBrushFromTexture(TextureToShow, false);
	InteractionImage->SetVisibility(ESlateVisibility::Visible);
	ShowInteractionNotifyWidget();
}
