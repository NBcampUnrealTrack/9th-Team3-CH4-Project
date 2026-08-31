// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameInstance.h"

#include "Kismet/KismetSystemLibrary.h"

void UPS3GameInstance::SetControllerEType(const FUniqueNetIdRepl& SetPlayerID, EPS3PlayerRoleType SetControllerType)
{
	ControllerTypesMap.Add(SetPlayerID, SetControllerType);
}

EPS3PlayerRoleType UPS3GameInstance::GetControllerEType(const FUniqueNetIdRepl& FindPlayerID)
{
	if (FindPlayerID.IsValid() == false) return EPS3PlayerRoleType::PlayerRole_None;
	
	EPS3PlayerRoleType* FoundedControllerType = ControllerTypesMap.Find(FindPlayerID);
	if (FoundedControllerType == nullptr) return EPS3PlayerRoleType::PlayerRole_None;
	
	return *FoundedControllerType;
}



