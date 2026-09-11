#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "S5_GameRuleDelegateComponent.generated.h"

class UOverlapVolumeTimeDeductionComponent;

#pragma region TwoParms_Declare_Delegate
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAssignFakeGimmickIDForUI,UOverlapVolumeTimeDeductionComponent* TimeDeductionComp, int32 IndexNumber)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnIsInteractionGimmick, const UActorComponent*, bool)
#pragma endregion 


#pragma region OneParms_Declare_Delegate
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInteractedGimmick, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsControlDoorOpen, bool)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsGameStart, bool)
#pragma endregion 


#pragma region Normal_Declare_Delegate
DECLARE_MULTICAST_DELEGATE(FOnSpawnedScreenPlayerUIReAssign)
DECLARE_MULTICAST_DELEGATE(FOnScreenPlayerSpawned)
DECLARE_MULTICAST_DELEGATE(FOnStageClear)
#pragma endregion 


UCLASS()
class PUZZLESTAY3_API US5_GameRuleDelegateComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	US5_GameRuleDelegateComponent()
	{
		SetIsReplicatedByDefault(true);
	}
	
	FOnAssignFakeGimmickIDForUI OnAssignFakeGimmickIDForUI;
	FOnIsInteractionGimmick OnIsInteractionGimmick;
	FOnSpawnedScreenPlayerUIReAssign OnSpawnedScreenPlayerUIReAssign;
	FOnIsControlDoorOpen OnIsControlDoorOpen;
	FOnInteractedGimmick OnInteractedGimmick;
	
#pragma region TwoParms_Delegate
	
	
#pragma endregion 
	
	
#pragma region OneParms_Delegate
	FOnIsGameStart OnIsGameStart;
	
#pragma endregion 
		
	
#pragma region Nomal_Delegate
	FOnScreenPlayerSpawned OnScreenPlayerSpawned;
#pragma endregion 
	
};


#pragma region Broadcast & Binding Macro
#define PS3_S5_GAME_RULE_DELEGATE_BROADCAST_TwoParams(DelegateVar, OneParams, TwoParms)\
{ \
	if (UWorld* World = GetWorld())\
	{ \
		if (APS3GameStateS5* PS3GameStateS5_Delegate = Cast<APS3GameStateS5>(World->GetGameState())) \
		{ \
			if (IsValid(PS3GameStateS5_Delegate->GetDelegate_S5()) == true)\
			{ PS3GameStateS5_Delegate->GetDelegate_S5()->DelegateVar.Broadcast(OneParams, TwoParms);} \
		} \
	} \
}

#define PS3_S5_GAME_RULE_DELEGATE_BROADCAST_OneParams(DelegateVar, OneParams)\
{ \
	if (UWorld* World = GetWorld())\
	{ \
		if (APS3GameStateS5* PS3GameStateS5_Delegate = Cast<APS3GameStateS5>(World->GetGameState())) \
		{ \
			if (IsValid(PS3GameStateS5_Delegate->GetDelegate_S5()) == true)\
			{ PS3GameStateS5_Delegate->GetDelegate_S5()->DelegateVar.Broadcast(OneParams);} \
		} \
	} \
}
	
#define PS3_S5_GAME_RULE_DELEGATE_BROADCAST(DelegateVar)\
{ \
	if (UWorld* World = GetWorld())\
	{ \
		if (APS3GameStateS5* PS3GameStateS5_Delegate = Cast<APS3GameStateS5>(World->GetGameState())) \
		{ \
			if (IsValid(PS3GameStateS5_Delegate->GetDelegate_S5()) == true)\
			{ PS3GameStateS5_Delegate->GetDelegate_S5()->DelegateVar.Broadcast();} \
		} \
	} \
}


#define PS3_S5_GAME_RULE_DELEGATE_BINDING_FUNCTION(DelegateVar, BindingFunc) \
{ \
	if (UWorld* World = GetWorld())\
	{ \
		if (APS3GameStateS5* PS3GameStateS5_Delegate = Cast<APS3GameStateS5>(World->GetGameState())) \
		{ \
			if (IsValid(PS3GameStateS5_Delegate->GetDelegate_S5()) == true)\
			{ PS3GameStateS5_Delegate->GetDelegate_S5()->DelegateVar.AddUObject(this, &ThisClass::BindingFunc);} \
		} \
	} \
}
#pragma endregion 
	


