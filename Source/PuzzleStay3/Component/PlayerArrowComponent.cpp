// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerArrowComponent.h"

UPlayerArrowComponent::UPlayerArrowComponent()
{
	this->SetRelativeScale3D(FVector(12.0f,12.0f,12.0f));
	this->SetVisibility(false);
	this->SetCastContactShadow(false);
	this->SetCastShadow(false);
}

void UPlayerArrowComponent::BeginPlay()
{
	Super::BeginPlay();
	
	checkf(IsValid(this->GetStaticMesh()) == true, TEXT("[PlayerArrowComponent] 스태틱 메쉬가 비어있습니다."));
}

