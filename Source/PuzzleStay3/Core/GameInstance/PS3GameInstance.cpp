// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameInstance.h"

bool UPS3GameInstance::TryGetCachedPlayerIdentity(
	const FString& PlayerKey,
	EPS3PlayerIdentity& OutIdentity) const
{
	if (const EPS3PlayerIdentity* CachedIdentity = PlayerIdentityByKey.Find(PlayerKey))
	{
		OutIdentity = *CachedIdentity;
		return true;
	}

	return false;
}

void UPS3GameInstance::CachePlayerIdentity(
	const FString& PlayerKey,
	const EPS3PlayerIdentity Identity)
{
	if (!PlayerKey.IsEmpty() && Identity != EPS3PlayerIdentity::Unassigned)
	{
		PlayerIdentityByKey.Add(PlayerKey, Identity);
	}
}