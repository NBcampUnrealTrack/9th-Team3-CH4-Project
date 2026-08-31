// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Dumbbell.h"

// Sets default values
ADumbbell::ADumbbell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADumbbell::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADumbbell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

