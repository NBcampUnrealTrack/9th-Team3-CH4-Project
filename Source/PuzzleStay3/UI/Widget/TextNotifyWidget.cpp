#include "TextNotifyWidget.h"

#include "Components/TextBlock.h"
#include "Engine/World.h"

void UTextNotifyWidget::ShowTextNotify(const FText& InDisplayText, float InFontSize, float InDisplayDuration)
{
	UE_LOG(
		LogTemp,
		Log,
		TEXT("RootHUDTest TextNotifyWidget::ShowTextNotify Entry Text=%s TextNotifyTextValid=%s"),
		*InDisplayText.ToString(),
		TextNotifyText ? TEXT("true") : TEXT("false")
	);

	if (TextNotifyText)
	{
		TextNotifyText->SetText(InDisplayText);

		FSlateFontInfo FontInfo = TextNotifyText->GetFont();
		FontInfo.Size = static_cast<int32>(InFontSize);
		TextNotifyText->SetFont(FontInfo);
		TextNotifyText->SetVisibility(ESlateVisibility::Visible);
		UE_LOG(
			LogTemp,
			Log,
			TEXT("RootHUDTest TextNotifyWidget::ShowTextNotify AfterSetTextAndVisible Text=%s Visibility=%d"),
			*TextNotifyText->GetText().ToString(),
			static_cast<int32>(TextNotifyText->GetVisibility())
		);
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
	if (TextNotifyText)
	{
		TextNotifyText->SetVisibility(ESlateVisibility::Collapsed);
	}
}
