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
	RandomCollisionLayoutResults.Empty();
	
	int32 RandomCollisionTrapCount = RandomCollisionTrapComponent.Num();
	
	for (int32 i = 0; i < RandomCollisionTrapCount/2; i++)
	{
		RandomCollisionResults.Add(FMath::RandBool());
	}
	
	for (int32 i = 0; i < RandomCollisionResults.Num(); i++)
	{
		if (RandomCollisionResults[i] == true)
		{
			RandomCollisionLayoutResults.Add(true);
			RandomCollisionLayoutResults.Add(false);
		}
		else
		{
			RandomCollisionLayoutResults.Add(false);
			RandomCollisionLayoutResults.Add(true);
		}
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

TArray<bool> APS3GameModeS2::GetRandomCollisionLayoutResults()
{
	return RandomCollisionLayoutResults;
}


