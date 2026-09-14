// Fill out your copyright notice in the Description page of Project Settings.


#include "EndingVideoWidget.h"

#include "MediaPlayer.h"
#include "MediaSource.h"

void UEndingVideoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!IsValid(MediaPlayer) || !IsValid(MediaSource))
	{
		return;
	}

	MediaPlayer->OnEndReached.AddDynamic(this, &ThisClass::HandleVideoEnded);
	MediaPlayer->OpenSource(MediaSource);
}

void UEndingVideoWidget::HandleVideoEnded()
{
	// 예: 타이틀로 돌아가기
	// UGameplayStatics::OpenLevel(this, TEXT("TitleLevelName"));
}