// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndingVideoWidget.generated.h"

class UMediaPlayer;
class UMediaSource;

UCLASS()
class PUZZLESTAY3_API UEndingVideoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EndingVideo")
	TObjectPtr<UMediaPlayer> MediaPlayer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EndingVideo")
	TObjectPtr<UMediaSource> MediaSource;

private:
	UFUNCTION()
	void HandleVideoEnded();
};
