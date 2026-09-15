#include "TextNotifyWidget.h"

#include "Components/TextBlock.h"
#include "Engine/World.h"

namespace
{
	constexpr float DefaultTextNotifyFontSize = 32.0f;
	constexpr float StageTextNotifyFontSize = 64.0f;
	constexpr float StageTextNotifyDuration = 3.0f;
}

void UTextNotifyWidget::ShowTextNotify(EPS3TextNotifyType NotifyType)
{
	switch (NotifyType)
	{
	case EPS3TextNotifyType::Stage1:
		ShowTextNotifyInternal(FText::FromString(TEXT("스테이지 1")), StageTextNotifyFontSize, StageTextNotifyDuration);
		break;
	case EPS3TextNotifyType::Stage2:
		ShowTextNotifyInternal(FText::FromString(TEXT("스테이지 2")), StageTextNotifyFontSize, StageTextNotifyDuration);
		break;
	case EPS3TextNotifyType::Stage3:
		ShowTextNotifyInternal(FText::FromString(TEXT("스테이지 3")), StageTextNotifyFontSize, StageTextNotifyDuration);
		break;
	case EPS3TextNotifyType::Stage4:
		ShowTextNotifyInternal(FText::FromString(TEXT("스테이지 4")), StageTextNotifyFontSize, StageTextNotifyDuration);
		break;
	case EPS3TextNotifyType::Stage5:
		ShowTextNotifyInternal(FText::FromString(TEXT("스테이지 5")), StageTextNotifyFontSize, StageTextNotifyDuration);
		break;
	case EPS3TextNotifyType::DoorOpenedSound:
		ShowTextNotifyInternal(FText::FromString(TEXT("어디선가 문이 열리는 소리가 났다.")), DefaultTextNotifyFontSize, StageTextNotifyDuration);
		break;
	case EPS3TextNotifyType::DoorClosedSound:
		ShowTextNotifyInternal(FText::FromString(TEXT("어디선가 문이 닫히는 소리가 났다.")), DefaultTextNotifyFontSize, StageTextNotifyDuration);
		break;
	case EPS3TextNotifyType::RadioBroken:
		ShowTextNotifyInternal(FText::FromString(TEXT("무전기가 고장난 것 같다.")), DefaultTextNotifyFontSize, StageTextNotifyDuration);
		break;
	case EPS3TextNotifyType::RadioAcquired:
		ShowTextNotifyInternal(FText::FromString(TEXT("무전기를 얻었다. 하지만 상태가 좋지 않은 것 같다.")), DefaultTextNotifyFontSize, StageTextNotifyDuration);
		break;
	case EPS3TextNotifyType::Radiorepair:
		ShowTextNotifyInternal(FText::FromString(TEXT("무전기 상태가 좋아진 것 같다.")), DefaultTextNotifyFontSize, StageTextNotifyDuration);
		break;
	case EPS3TextNotifyType::BlockVolume:
		ShowTextNotifyInternal(FText::FromString(TEXT("가기 전에 먼저 해야 할 일이 있는 것 같다.")), DefaultTextNotifyFontSize, StageTextNotifyDuration);
		break;
	case EPS3TextNotifyType::None:
	default:
		break;
	}
}

void UTextNotifyWidget::ShowTextNotifyInternal(const FText& InDisplayText, float InFontSize, float InDisplayDuration)
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

void UTextNotifyWidget::SetTextNotifyVisible(bool bVisible)
{
	if (bVisible)
	{
		SetVisibility(ESlateVisibility::Visible);

		if (TextNotifyText)
		{
			TextNotifyText->SetVisibility(ESlateVisibility::Visible);
		}
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
