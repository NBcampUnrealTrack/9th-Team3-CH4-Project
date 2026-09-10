#include "OverlapVolumeTimeDeductionComponent.h"

#include "EngineUtils.h"
#include "Core/GameMode/PS3GameModeS5.h"
#include "Core/GameState/PS3GameStateS5.h"
#include "Data/Delegates/UIDelegatesSubsystem.h"
#include "Player/Character/PS3PlayerCharacter.h"
#include "Player/Controller/PS3PlayerController.h"


UOverlapVolumeTimeDeductionComponent::UOverlapVolumeTimeDeductionComponent()
{
}


void UOverlapVolumeTimeDeductionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(GetCastPS3GameModeS5()) == false) return;
	CastPS3GameModeS5->OnIsGameStart.AddUObject(this, &ThisClass::OnBindWhenGameStarted);
	
	//ErrorCheck_S5();
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
		if (IsValid(CastPS3GameModeS5)) return CastPS3GameModeS5;

		if (UWorld* World = GetWorld())
		{
			CastPS3GameModeS5 = Cast<APS3GameModeS5>(World->GetAuthGameMode());
			return CastPS3GameModeS5;
		}
	}
	
	return nullptr;
}


void UOverlapVolumeTimeDeductionComponent::OnCharacterOverLapped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsInteractionGimmick ==  true) return;
	
	if (GetOwner() == nullptr) return;
	
	if (GetOwner()->HasAuthority() == true)
	{
		auto* PS3PlayerCharacter = Cast<APS3PlayerCharacter>(OtherActor);
		if (IsValid(PS3PlayerCharacter) == false) return;
		
		auto* PS3PlayerController = Cast<APS3PlayerController>(PS3PlayerCharacter->GetController());
		if (IsValid(PS3PlayerController) == false) return;
		
		if (OverlappedCharacters.Contains(PS3PlayerCharacter) == true) return;
		OverlappedCharacters.Add(PS3PlayerCharacter);
		
		if (IsValid(CastPS3GameModeS5) == false) return;
		
		NetMultiRPC_RemindingTimeDeduct();
		
		CastPS3GameModeS5->OnTimeDeduction(DeductedTimeRange);
	
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


void UOverlapVolumeTimeDeductionComponent::NetMultiRPC_RemindingTimeDeduct_Implementation()
{
	PS3_BROADCAST_TO_MVVM_TwoParams(OnTimeDeduct_UI, TimeDeductTimerUIType, DeductedTimeRange);
}


void UOverlapVolumeTimeDeductionComponent::ReSpawnPlayer(APlayerController* TargetPlayerController)
{
	if (IsValid(TargetPlayerController) == false) return;
	
	auto* PS3GameStateS5 = Cast<APS3GameStateS5>(GetWorld()->GetGameState());
	if (IsValid(PS3GameStateS5) == false) return;
	
	PS3GameStateS5->ReSpawnPlayer(TargetPlayerController);
}

void UOverlapVolumeTimeDeductionComponent::ErrorCheck_S5()
{
	if (GetOwner() == nullptr) return;
	
	if (GetOwner()->HasAuthority() == false) return;
	
	int32 TotalGimmickBaseCount = 0;
	int32 InvalidEnumCount = 0;

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* GimmickBase = *It;
		if (IsValid(GimmickBase) == false) continue;
		
		TotalGimmickBaseCount++;
		auto* UOverlapVolumeTimeDeductionComp = GimmickBase->FindComponentByClass<UOverlapVolumeTimeDeductionComponent>();
		if (IsValid(UOverlapVolumeTimeDeductionComp) == false) continue;
			
		if (UOverlapVolumeTimeDeductionComp->TimeDeductTimerUIType == EPS3TimerUIType::None)
		{
			InvalidEnumCount++;
		}
	}

	checkf(InvalidEnumCount == 0, 
		TEXT("Type 선정 오류: 전체 %d 개의 [UOverlapVolumeTimeDeductionComponent] 중 %d개의 Type이 <None>입니다. Type을 선정해주세요."), 
		TotalGimmickBaseCount, InvalidEnumCount);
}
