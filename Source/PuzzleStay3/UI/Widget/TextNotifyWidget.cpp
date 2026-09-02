#include "TextNotifyWidget.h"

#include "Components/TextBlock.h"
#include "Engine/World.h"

void UTextNotifyWidget::ShowTextNotify(const FText& InDisplayText, float InFontSize, float InDisplayDuration)
{
	if (TextNotifyText)
	{
		SetVisibility(ESlateVisibility::Visible);
		TextNotifyText->SetText(InDisplayText);

		FSlateFontInfo FontInfo = TextNotifyText->GetFont();
		FontInfo.Size = static_cast<int32>(InFontSize);
		TextNotifyText->SetFont(FontInfo);
		TextNotifyText->SetVisibility(ESlateVisibility::Visible);
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	World->GetTimerManager().ClearTimer(TextNotifyTimerHandle);

	if (InDisplayDuration > 0.0f)
	{
		World->GetTimerManager().SetTimer(TextNotifyTimerHandle, this, &UTextNotifyWidget::HideTextNotify, InDisplayDuration, false);
	}
	else
	{
		HideTextNotify();
	}
}

void UTextNotifyWidget::HideTextNotify()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TextNotifyTimerHandle);
	}

	if (TextNotifyText)
	{
		TextNotifyText->SetVisibility(ESlateVisibility::Collapsed);
	}

	SetVisibility(ESlateVisibility::Collapsed);
}
