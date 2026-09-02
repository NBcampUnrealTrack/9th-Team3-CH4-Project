#include "InteractionNotifyWidget.h"

#include "Components/Image.h"

void UInteractionNotifyWidget::ShowInteractionNotify(FName InNotifyId, const FText& InKeyName)
{
	if (InNotifyId.IsNone())
	{
		return;
	}

	ActiveInteractionNotifies.Add(InNotifyId, InKeyName);
	RefreshInteractionImage();
}

void UInteractionNotifyWidget::HideInteractionNotify(FName InNotifyId)
{
	if (InNotifyId.IsNone())
	{
		return;
	}

	ActiveInteractionNotifies.Remove(InNotifyId);
	RefreshInteractionImage();
}

void UInteractionNotifyWidget::HideAllInteractionNotifies()
{
	ActiveInteractionNotifies.Empty();
	RefreshInteractionImage();
}

void UInteractionNotifyWidget::RefreshInteractionImage()
{
	if (!InteractionImage)
	{
		return;
	}

	UTexture2D* TextureToShow = nullptr;
	bool bHasFInteraction = false;

	for (const TPair<FName, FText>& ActiveInteractionNotify : ActiveInteractionNotifies)
	{
		const FString KeyName = ActiveInteractionNotify.Value.ToString();
		if (KeyName.Equals(TEXT("G"), ESearchCase::IgnoreCase))
		{
			TextureToShow = GInteractionTexture;
			break;
		}

		if (KeyName.Equals(TEXT("F"), ESearchCase::IgnoreCase))
		{
			bHasFInteraction = true;
		}
	}

	if (!TextureToShow && bHasFInteraction)
	{
		TextureToShow = FInteractionTexture;
	}

	if (!TextureToShow)
	{
		InteractionImage->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	InteractionImage->SetBrushFromTexture(TextureToShow, false);
	InteractionImage->SetVisibility(ESlateVisibility::Visible);
}
