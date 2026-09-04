// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameModeS2.h"

#include "Component/RandomCollisionTrapComponent.h"

void APS3GameModeS2::BeginPlay()
{
	Super::BeginPlay();
	
	MakeRandomCollisionResults();
	
}

void APS3GameModeS2::MakeRandomCollisionResults()
{
	RandomCollisionResults.Empty();
	
	int32 RandomCollisionTrapCount = RandomCollisionTrapComponent.Num();
	
	for (int32 i = 0; i < RandomCollisionTrapCount; i++)
	{
		RandomCollisionResults.Add(FMath::RandBool());
	}
}

void APS3GameModeS2::RegisterRandomCollisionTrapCompo(URandomCollisionTrapComponent* TrapComponent)
{
	if (!TrapComponent) return;

	RandomCollisionTrapComponent.AddUnique(TrapComponent);

	//UE_LOG(LogTemp, Warning, TEXT("Registered TrapCompo. Count: %d"), RandomCollisionTrapComponent.Num());
}

void APS3GameModeS2::UnregisterRandomCollisionTrapCompo(URandomCollisionTrapComponent* TrapComponent)
{
	if (!TrapComponent) return;

	RandomCollisionTrapComponent.Remove(TrapComponent);
}

bool APS3GameModeS2::GetRandomCollisionResult(int32 RowIndex)
{
	if (!RandomCollisionResults.IsValidIndex(RowIndex)) {return false;}

	return RandomCollisionResults[RowIndex];
}



//C++ Class - GimmickBase
//BP Class - BP_RandomCollisionTrap(RandomCollisionTrapCompo)

//LEVEL
BP_RandomCollisionTrap(bool isleft) | BP_RandomCollisionTrap(bool isleft)
BP_RandomCollisionTrap | BP_RandomCollisionTrap
BP_RandomCollisionTrap | BP_RandomCollisionTrap
BP_RandomCollisionTrap | BP_RandomCollisionTrap
BP_RandomCollisionTrap | BP_RandomCollisionTrap
BP_RandomCollisionTrap | BP_RandomCollisionTrap
BP_RandomCollisionTrap | BP_RandomCollisionTrap
BP_RandomCollisionTrap | BP_RandomCollisionTrap
BP_RandomCollisionTrap | BP_RandomCollisionTrap
