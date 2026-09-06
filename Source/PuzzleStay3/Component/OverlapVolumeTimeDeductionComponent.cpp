#include "OverlapVolumeTimeDeductionComponent.h"

#include "Core/GameMode/PS3GameModeS5.h"
#include "Core/GameState/PS3GameStateS5.h"
#include "Player/Character/PS3PlayerCharacter.h"
#include "Player/Controller/PS3PlayerController.h"


UOverlapVolumeTimeDeductionComponent::UOverlapVolumeTimeDeductionComponent()
{
}


void UOverlapVolumeTimeDeductionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetCastPS3GameModeS5() == nullptr) return;
	CastPS3GameModeS5->OnIsGameStart.AddUObject(this, &ThisClass::OnBindWhenGameStarted);
}


void UOverlapVolumeTimeDeductionComponent::OnBindWhenGameStarted(bool bIsGameStart)
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
		
		if (IsValid(CastPS3GameModeS5) == false) return nullptr;
		
		CastPS3GameModeS5 = Cast<APS3GameModeS5>(World->GetAuthGameMode());
		return CastPS3GameModeS5;
	}
	
	return nullptr;
}


void UOverlapVolumeTimeDeductionComponent::OnCharacterOverLapped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsEscapeDoor ==  true) return;
	
	if (GetOwner() == nullptr) return;
	
	if (GetOwner()->HasAuthority() == true)
	{
		auto* PS3PlayerCharacter = Cast<APS3PlayerCharacter>(OtherActor);
		if (IsValid(PS3PlayerCharacter) == false) return;
		
		auto* PS3PlayerController = Cast<APS3PlayerController>(PS3PlayerCharacter->GetController());
		if (IsValid(PS3PlayerController) == false) return;
		
		if (OverlappedCharacters.Contains(PS3PlayerCharacter) == true) return;
		OverlappedCharacters.Add(PS3PlayerCharacter);
		
		auto* PS3GameModeS5 = GetCastPS3GameModeS5();
		if (IsValid(PS3GameModeS5) == false) return;
		
		PS3GameModeS5->OnTimeDeduction(DeductedTimeRange);
	
		ReSpawnPlayer(PS3PlayerController);
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


void UOverlapVolumeTimeDeductionComponent::ReSpawnPlayer(APlayerController* TargetPlayerController)
{
	if (IsValid(TargetPlayerController) == false) return;
	
	auto* PS3GameStateS5 = Cast<APS3GameStateS5>(GetWorld()->GetGameState());
	if (IsValid(PS3GameStateS5) == false) return;
	
	PS3GameStateS5->ReSpawnPlayer(TargetPlayerController);
}



