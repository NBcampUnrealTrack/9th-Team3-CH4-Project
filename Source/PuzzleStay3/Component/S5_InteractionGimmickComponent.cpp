#include "S5_InteractionGimmickComponent.h"

#include "Components/BoxComponent.h"
#include "Core/GameMode/PS3GameModeS5.h"
#include "Core/GameState/PS3GameStateS5.h"
#include "Data/Delegates/UIDelegatesSubsystem.h"
#include "Data/Enum/PS3InteractionNotifyType.h"
#include "Net/UnrealNetwork.h"
#include "Player/Character/PS3PlayerCharacter.h"


US5_InteractionGimmickComponent::US5_InteractionGimmickComponent()
{
	//if (GetOwner() == nullptr) return;
	
	SetIsReplicatedByDefault(true);
	
	InitBoxExtent(FVector(140.0f, 140.0f, 140.0f));
}


void US5_InteractionGimmickComponent::BeginPlay()
{
	Super::BeginPlay();
	
	this->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnCharacterBeginOverlapForUI);
	this->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnCharacterEndOverlapForUI);
	
	auto* PS3GameModeS5 = Cast<APS3GameModeS5>(GetWorld()->GetAuthGameMode());
	if (IsValid(PS3GameModeS5) == false) return;
	PS3GameModeS5->OnIsInteractionGimmick.AddUObject(this, &ThisClass::OnColletedGimmickBase);
	PS3GameModeS5->OnIsGameStart.AddUObject(this, &ThisClass::OnStartedGame);
}


void US5_InteractionGimmickComponent::GetLifetimeReplicatedProps(
	TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, bIsInteractionGimmick);
	DOREPLIFETIME(ThisClass, bIsInteracted);
	DOREPLIFETIME(ThisClass, bIsStartedGame);
}


bool US5_InteractionGimmickComponent::CanInteract_Implementation(AActor* Requestor) const
{
	if (bIsStartedGame == false) return false;
	
	return true;
}


void US5_InteractionGimmickComponent::OnStartedGame(bool bIsGameStart)
{
	bIsStartedGame = bIsGameStart;
}


bool US5_InteractionGimmickComponent::Interact_Implementation(AActor* Requestor)
{
	if (bIsInteractionGimmick == false) return false;
	
	if (bIsInteracted == true) return false;
	
	OnCosmeticInteractionSuccessed.Broadcast();
	OnInteractionGimmick.Broadcast(bIsInteracted);
	bIsInteracted = true;
	
	return true;
}


void US5_InteractionGimmickComponent::OnCharacterBeginOverlapForUI(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsInteractionGimmick == false) return;
	
	if (GetOwner() == nullptr) return;
	
	auto* PS3PlayerCharacter = Cast<APS3PlayerCharacter>(OtherActor);
	if (IsValid(PS3PlayerCharacter) == false) return;
	
	
	if (OverlappedCharacters.Contains(PS3PlayerCharacter) == true) return;
	OverlappedCharacters.Add(PS3PlayerCharacter);
	
	
	if (PS3PlayerCharacter->IsLocallyControlled() == true)
	{
		GetWorld()->GetTimerManager().SetTimer(CheckPlayerTimerHandle, this, 
			&US5_InteractionGimmickComponent::CheckCanDisplayedUIForTimer, 0.1f, true);
	}
	
}


void US5_InteractionGimmickComponent::CheckCanDisplayedUIForTimer()
{
	if (bIsInteractionGimmick == false) return;
	
	bool bIsCanDisplayedUI = CheckCanDisplayedUI();
	
	if (bIsCanDisplayedUI == true && bIsUIVisible == false)
	{
		PS3_BROADCAST_TO_MVVM_TwoParams(OnInteractRequestS5_UI,EPS3InteractionNotifyType::Interact, true);
		bIsUIVisible = true;
	}
	else if (bIsCanDisplayedUI == false  && bIsUIVisible == true)
	{
		PS3_BROADCAST_TO_MVVM_TwoParams(OnInteractRequestS5_UI,EPS3InteractionNotifyType::Interact, false);
		bIsUIVisible = false;
	}
}


bool US5_InteractionGimmickComponent::CheckCanDisplayedUI()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (IsValid(PlayerController) == false) return false;
	
	auto* PS3PlayerCharacter = Cast<APS3PlayerCharacter>(PlayerController->GetPawn());
	if (IsValid(PS3PlayerCharacter) == false) return false;
	
	if (GetOwner() == nullptr) return false;
	
	if (OverlappedCharacters.Contains(PS3PlayerCharacter) == false) return false;
	
	FVector DirectionToGimmick = GetOwner()->GetActorLocation() - PS3PlayerCharacter->GetActorLocation();
	DirectionToGimmick.Z = 0.0f;
	DirectionToGimmick.Normalize();
		
	FVector PlayerForwardVector = PS3PlayerCharacter->GetActorForwardVector();
	PlayerForwardVector.Z = 0.0f;
	PlayerForwardVector.Normalize();
		
	float PlayerDotValue = FVector::DotProduct(PlayerForwardVector, DirectionToGimmick);
	float GimmickFOVAngle = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(PlayerDotValue, -1.0f, 1.0f)));
	
	return GimmickFOVAngle <= (TargetFOVAngle * 0.5f);
}


void US5_InteractionGimmickComponent::OnCharacterEndOverlapForUI(UPrimitiveComponent* OverlappedComp,
                                                                 AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//if (bIsInteractionGimmick == false) return;
	
	if (GetOwner() == nullptr) return;
	
	auto* PS3PlayerCharacter = Cast<APS3PlayerCharacter>(OtherActor);
	if (IsValid(PS3PlayerCharacter) == false) return;
	
	OverlappedCharacters.Remove(PS3PlayerCharacter);
	
	if (PS3PlayerCharacter->IsLocallyControlled() == true)
	{
		GetWorld()->GetTimerManager().ClearTimer(CheckPlayerTimerHandle);
		if (bIsUIVisible == true)
		{
			PS3_BROADCAST_TO_MVVM_TwoParams(OnInteractRequestS5_UI,EPS3InteractionNotifyType::Interact, false);
			bIsUIVisible = false;
		}
	}
}


void US5_InteractionGimmickComponent::OnColletedGimmickBase(const UBoxComponent* CurrentComponent, bool bIsInteractable)
{
	if (this == CurrentComponent)
	{
		bIsInteractionGimmick = bIsInteractable;
	}
}

