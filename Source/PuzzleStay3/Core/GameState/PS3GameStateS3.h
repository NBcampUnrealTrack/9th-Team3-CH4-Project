// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PS3GameStateBase.h"
#include "PS3GameStateS3.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStage3BlockingVolumeDisabled, bool, bDisabled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStage3VoiceChatActivated, bool, bActivated);

UCLASS()
class PUZZLESTAY3_API APS3GameStateS3 : public APS3GameStateBase
{
	GENERATED_BODY()

	//BlockingVolume
public:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void InitializeToDataAssets() override;
	
	UPROPERTY(EditAnywhere, Category = "GameRule")
	TObjectPtr<class US3_GameRuleDataAsset> S3_GameRuleDataAsset;

public:
	UFUNCTION(BlueprintPure)
	bool IsStage3BlockingVolumeDisabled() const { return bStage3BlockingVolumeDisabled; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetStage3BlockingVolumeDisabled(bool bDisabled);

	UPROPERTY(BlueprintAssignable)
	FOnStage3BlockingVolumeDisabled OnStage3BlockingVolumeDisabled;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Stage3BlockingVolumeDisabled)
	bool bStage3BlockingVolumeDisabled = false;

	UFUNCTION()
	void OnRep_Stage3BlockingVolumeDisabled();
	
	//VoiceChat
public:
	UFUNCTION(BlueprintPure)
	bool IsStage3VoiceChatActivated() const { return bStage3VoiceChatActivated; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetStage3VoiceChatActivated(bool bDisabled);

	UPROPERTY(BlueprintAssignable)
	FOnStage3VoiceChatActivated OnStage3VoiceChatActivated;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Stage3VoiceChatActivated)
	bool bStage3VoiceChatActivated = false;

	UFUNCTION()
	void OnRep_Stage3VoiceChatActivated();
	
public:
	virtual void NotifyPlayerDeadGameOver() override;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_PlayerDeadGameOver)
	bool bPlayerDeadGameOver = false;

	UFUNCTION()
	void OnRep_PlayerDeadGameOver();
};