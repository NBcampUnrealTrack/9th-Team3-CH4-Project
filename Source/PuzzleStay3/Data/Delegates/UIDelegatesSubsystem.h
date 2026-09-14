// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/PS3TextNotifyType.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UIDelegatesSubsystem.generated.h"


enum class EPS3InteractionNotifyType : uint8;
enum class EControlDoorType : uint8;
enum class EPS3TimerUIType : uint8;
enum class EPS3StageType : uint8;


#pragma region TwoParms_Declare_Delegate
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnButtonEnabled_UI, EControlDoorType, bool)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGameTimer_UI, EPS3TimerUIType, float)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTimeDeductUI, EPS3TimerUIType, float)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLifeCount_UI, int32, int32)
#pragma endregion 


#pragma region OneParms_Declare_Delegate
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInteractionNotifyRemoveRequested_UI, EPS3InteractionNotifyType)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInteractionNotifyAddRequested_UI, EPS3InteractionNotifyType)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTextNotify_UI, EPS3TextNotifyType)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnStageType_UI, EPS3StageType)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTimerNotifyVisible_UI, bool)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTextNotifyVisible_UI, bool)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRoleSelection_UI, bool)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnVoiceChatIcon_UI, bool)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnScreenPlayer_UI, bool)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnOptionPopup_UI, bool)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnFieldPlayer_UI, bool)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsGameOver_UI, bool)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTutorial_UI, bool)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTitle_UI, bool)
#pragma endregion 


#pragma region Normal_Declare_Delegate
DECLARE_MULTICAST_DELEGATE(FOnInteractionNotifyResetRequested_UI)
DECLARE_MULTICAST_DELEGATE(FOnTimerReset_UI)
#pragma endregion 


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
#pragma region TwoParms_Delegate
	FOnButtonEnabled_UI OnButtonEnabled_UI;
	FOnGameTimer_UI OnGameTimer_UI;
	FOnTimeDeductUI OnTimeDeduct_UI;
	FOnLifeCount_UI OnLifeCount_UI;
#pragma endregion 
	
	
#pragma region OneParms_Delegate
	FOnInteractionNotifyRemoveRequested_UI OnInteractionNotifyRemoveRequested_UI;
	FOnInteractionNotifyAddRequested_UI OnInteractionNotifyAddRequested_UI;
	FOnTextNotify_UI OnTextNotify_UI;
	FOnStageType_UI OnStageType_UI;
	FOnTimerNotifyVisible_UI OnTimerNotifyVisible_UI;
	FOnTextNotifyVisible_UI OnTextNotifyVisible_UI;
	FOnRoleSelection_UI OnRoleSelection_UI;
	FOnVoiceChatIcon_UI OnVoiceChatIcon_UI;
	FOnScreenPlayer_UI OnScreenPlayer_UI;
	FOnOptionPopup_UI OnOptionPopup_UI;
	FOnFieldPlayer_UI OnFieldPlayer_UI;
	FOnIsGameOver_UI OnIsGameOver_UI;
	FOnTutorial_UI OnTutorial_UI;
	FOnTitle_UI OnTitle_UI;
#pragma endregion 
		
	
#pragma region Nomal_Delegate
	FOnInteractionNotifyResetRequested_UI OnInteractionNotifyResetRequested_UI;
	FOnTimerReset_UI OnTimerReset_UI;
#pragma endregion 
	
};


#pragma region Broadcast & Binding Macro

#define PS3_BROADCAST_TO_MVVM_TwoParams(DelegateVar, OneParams, TwoParms)\
	{\
		auto* UIManager = UUIDelegatesSubsystem::GetUIDelegateManager(this);\
		if (IsValid(UIManager) == true) {UIManager->DelegateVar.Broadcast(OneParams, TwoParms);}\
	}

#define PS3_BROADCAST_TO_MVVM_OneParams(DelegateVar, OneParams)\
	{\
		auto* UIManager = UUIDelegatesSubsystem::GetUIDelegateManager(this);\
		if (IsValid(UIManager) == true) {UIManager->DelegateVar.Broadcast(OneParams);}\
	}
	
#define PS3_BROADCAST_TO_MVVM(DelegateVar)\
	{\
		auto* UIManager = UUIDelegatesSubsystem::GetUIDelegateManager(this);\
		if (IsValid(UIManager) == true) {UIManager->DelegateVar.Broadcast();}\
	}


#define PS3_BINDING_FUNCTION_TO_UIDELEGATE(DelegateVar, BindingFunc) \
	{ \
		auto* UIManager = UUIDelegatesSubsystem::GetUIDelegateManager(this); \
		if (IsValid(UIManager) == true) { UIManager->DelegateVar.AddUObject(this, &ThisClass::BindingFunc); } \
	}


#define PS3_UIDELEGATE_TIMER_FOR_MACRO(PS3_UIDelegateMacro)\
	{\
		FTimerHandle TempTimerHandle;\
		\
		GetWorld()->GetTimerManager().SetTimer(\
			TempTimerHandle,\
			FTimerDelegate::CreateLambda([this, &TempTimerHandle]()\
			{\
				PS3_UIDelegateMacro;\
				GetWorld()->GetTimerManager().ClearTimer(TempTimerHandle);\
			}),\
		0.1f, false);\
	}

#pragma endregion 
	


