#include "OverlapVolumeTimeDeductionComponent.h"

#include "Core/GameMode/PS3GameModeS5.h"
#include "Player/Character/PS3PlayerCharacter.h"


UOverlapVolumeTimeDeductionComponent::UOverlapVolumeTimeDeductionComponent()
{
}

void UOverlapVolumeTimeDeductionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OnGameStartedBind();
}



void UOverlapVolumeTimeDeductionComponent::OnGameStartedBind()
{
	if (GetCastPS3GameModeS5() == nullptr) return;
	GetCastPS3GameModeS5()->OnIsGameStart.AddUObject(this, &ThisClass::OnBindFunctionToComponent);
	
	//OnBindFunctionToComponent(true);
}

void UOverlapVolumeTimeDeductionComponent::OnBindFunctionToComponent(bool bIsGameStart)
{
	if (bIsGameStart == true)
	{
		OnComponentBeginOverlap.RemoveDynamic(this, &ThisClass::OnCharacterOverLapped);
		OnComponentEndOverlap.RemoveDynamic(this, &ThisClass::OnCharacterEndOverlap);
		
		OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnCharacterOverLapped);
		OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnCharacterEndOverlap);
	}
}


APS3GameModeS5* UOverlapVolumeTimeDeductionComponent::GetCastPS3GameModeS5()
{
	if (IsValid(GetOwner()) == false) return nullptr;
	
	if (GetOwner()->HasAuthority() == true)
	{
		UWorld* World = GetWorld();
		if (World == nullptr) return nullptr;
		
		PS3GameModeS5 = Cast<APS3GameModeS5>(World->GetAuthGameMode());
		return PS3GameModeS5;
	}
	
	return nullptr;
}

void UOverlapVolumeTimeDeductionComponent::OnCharacterOverLapped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetOwner() == nullptr) return;
	
	if (GetOwner()->HasAuthority() == true)
	{
		auto* PS3PlayerCharacter = Cast<APS3PlayerCharacter>(OtherActor);
		if (IsValid(PS3PlayerCharacter) == false) return;
		
		if (OverlappedCharacters.Contains(PS3PlayerCharacter) == true) return;
		OverlappedCharacters.Add(PS3PlayerCharacter);
		
		if (GetCastPS3GameModeS5() == nullptr) return;
		GetCastPS3GameModeS5()->OnTimeDeduction(DeductedTimeRange);
	}
}

void UOverlapVolumeTimeDeductionComponent::OnCharacterEndOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (GetOwner() == nullptr) return;
	
	if (GetOwner()->HasAuthority() == true)
	{
		if (IsValid(OtherActor) == false) return;
		if (OverlappedCharacters.Contains(OtherActor) == false) return;
		
		OverlappedCharacters.Remove(OtherActor);
	}
}


