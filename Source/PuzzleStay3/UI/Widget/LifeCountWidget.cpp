#include "LifeCountWidget.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "../ViewModel/PS3ViewModel.h"

void ULifeCountWidget::SetViewModel(UPS3ViewModel* InViewModel)
{
	ViewModel = InViewModel;
}

void ULifeCountWidget::ShowLifeCount()
{
	SetVisibility(ESlateVisibility::Visible);
}

void ULifeCountWidget::HideLifeCount()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void ULifeCountWidget::UpdateLifeCount(int32 InCurrentLifeCount, int32 InMaxLifeCount)
{
	if (!ViewModel)
	{
		return;
	}

	ViewModel->SetCurrentLifeCount(InCurrentLifeCount);
	ViewModel->SetMaxLifeCount(InMaxLifeCount);

	if (HeartImages.Num() != InMaxLifeCount)
	{
		RebuildHearts(InMaxLifeCount);
	}

	RefreshHearts(InCurrentLifeCount);
}

void ULifeCountWidget::RebuildHearts(int32 MaxLife)
{
	HeartImages.Empty();

	if (!HeartContainer)
	{
		return;
	}

	HeartContainer->ClearChildren();

	const int32 HeartCount = FMath::Max(0, MaxLife);
	for (int32 Index = 0; Index < HeartCount; ++Index)
	{
		UImage* HeartImage = NewObject<UImage>(this);
		if (!HeartImage)
		{
			continue;
		}

		FSlateBrush Brush = HeartImage->GetBrush();
		Brush.ImageSize = FVector2D(HeartSize, HeartSize);
		HeartImage->SetBrush(Brush);

		UHorizontalBoxSlot* HeartSlot = HeartContainer->AddChildToHorizontalBox(HeartImage);
		if (HeartSlot)
		{
			HeartSlot->SetPadding(FMargin(0.0f, 0.0f, HeartSpacing, 0.0f));
		}

		HeartImages.Add(HeartImage);
	}
}

void ULifeCountWidget::RefreshHearts(int32 CurrentLife)
{
	for (int32 Index = 0; Index < HeartImages.Num(); ++Index)
	{
		UImage* HeartImage = HeartImages[Index];
		if (!HeartImage)
		{
			continue;
		}

		UTexture2D* HeartTexture = Index < CurrentLife ? FilledHeartTexture : EmptyHeartTexture;

		HeartImage->SetBrushFromTexture(HeartTexture, false);

		FSlateBrush Brush = HeartImage->GetBrush();
		Brush.ImageSize = FVector2D(HeartSize, HeartSize);
		HeartImage->SetBrush(Brush);
	}
}
