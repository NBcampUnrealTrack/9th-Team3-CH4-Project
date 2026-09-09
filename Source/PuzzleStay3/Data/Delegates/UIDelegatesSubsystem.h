// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UIDelegatesSubsystem.generated.h"


enum class EControlDoorType : uint8;
enum class EPS3TimerUIType : uint8;
enum class EPS3StageType : uint8;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnButtonEnabled_UI, EControlDoorType, bool)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGameTimer_UI, EPS3TimerUIType, float)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTimeDeductUI, EPS3TimerUIType, float)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnStageType_UI, EPS3StageType)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRoleSelection_UI, bool)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnVoiceChatIcon_UI, bool)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnScreenPlayer_UI, bool)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnFieldPlayer_UI, bool)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsGameOver_UI, bool)

UCLASS()
class PUZZLESTAY3_API UUIDelegatesSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	static UUIDelegatesSubsystem* GetUIDelegateManager(const UObject* WorldContext)
	{
		if (WorldContext == nullptr) return nullptr;

		if (const UWorld* World = WorldContext->GetWorld())
		{
			if (UGameInstance* GI = World->GetGameInstance())
			{
				return GI->GetSubsystem<UUIDelegatesSubsystem>();
			}
		}
		return nullptr;
	}
	
public:
	FOnButtonEnabled_UI OnButtonEnabled_UI;
	FOnGameTimer_UI OnGameTimer_UI;
	FOnTimeDeductUI OnTimeDeduct_UI;
	FOnStageType_UI OnStageType_UI;
	FOnRoleSelection_UI OnRoleSelection_UI;
	FOnVoiceChatIcon_UI OnVoiceChatIcon_UI;
	FOnScreenPlayer_UI OnScreenPlayer_UI;
	FOnFieldPlayer_UI OnFieldPlayer_UI;
	FOnIsGameOver_UI OnIsGameOver_UI;
};

#define PS3_BROADCAST_TO_UI_TwoParams(DelegateVar, OneParams, TwoParms)\
	{\
		auto* UIManager = UUIDelegatesSubsystem::GetUIDelegateManager(this);\
		if (IsValid(UIManager) == true) {UIManager->DelegateVar.Broadcast(OneParams, TwoParms);}\
	}

#define PS3_BROADCAST_TO_UI_OneParams(DelegateVar, OneParams)\
	{\
		auto* UIManager = UUIDelegatesSubsystem::GetUIDelegateManager(this);\
		if (IsValid(UIManager) == true) {UIManager->DelegateVar.Broadcast(OneParams);}\
	}
	
#define PS3_BROADCAST_TO_UI(DelegateVar)\
	{\
		auto* UIManager = UUIDelegatesSubsystem::GetUIDelegateManager(this);\
		if (IsValid(UIManager) == true) {UIManager->DelegateVar.Broadcast();}\
	}


#define PS3_BIND_UI_DELEGATE_FUNCTION(DelegateVar, BindingFunc) \
	{ \
		auto* UIManager = UUIDelegatesSubsystem::GetUIDelegateManager(this); \
		if (IsValid(UIManager) == true) { UIManager->DelegateVar.AddUObject(this, &ThisClass::BindingFunc); } \
	}

	


