#include "S5_InteractionGimmickComponent.h"

#include "Components/BoxComponent.h"
#include "Core/GameMode/PS3GameModeS5.h"
#include "Core/GameState/PS3GameStateS5.h"
#include "Data/Delegates/UIDelegatesSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "Player/Character/PS3PlayerCharacter.h"


US5_InteractionGimmickComponent::US5_InteractionGimmickComponent()
{
	InteractionUIOverlapComponent = CreateDefaultSubobject<UBoxComponent>("InteractionUIOverlapComponent");
	InteractionUIOverlapComponent->SetupAttachment(GetOwner()->GetRootComponent());
	InteractionUIOverlapComponent->SetBoxExtent(FVector(70.0f, 70.0f, 0.0f));
	
}


void US5_InteractionGimmickComponent::BeginPlay()
{
	Super::BeginPlay();
	
	InteractionUIOverlapComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnCharacterBeginOverlapForUI);
	InteractionUIOverlapComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnCharacterEndOverlapForUI);
	
	
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
	DOREPLIFETIME(ThisClass, bIsInteractedGimmick);
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
	
	if (bIsInteractedGimmick == true) return false;
	
	OnCosmeticInteractionSuccessed.Broadcast();
	OnInteractionGimmick.Broadcast(bIsInteractedGimmick);
	bIsInteractedGimmick = true;
	
	return true;
}


void US5_InteractionGimmickComponent::OnCharacterBeginOverlapForUI(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetOwner() == nullptr) return;
	
	auto* PS3PlayerCharacter = Cast<APS3PlayerCharacter>(OtherActor);
	if (IsValid(PS3PlayerCharacter) == false) return;
	
	if (GetOwner()->HasAuthority() == true)
	{
		if (OverlappedCharacters.Contains(PS3PlayerCharacter) == true) return;
		OverlappedCharacters.Add(PS3PlayerCharacter);
	}
	
	if (PS3PlayerCharacter->IsLocallyControlled() == true)
	{
		FVector DirectionToGimmick = GetOwner()->GetActorLocation() - PS3PlayerCharacter->GetActorLocation();
		DirectionToGimmick.Z = 0.0f;
		DirectionToGimmick.Normalize();
		
		FVector PlayerForwardVector = PS3PlayerCharacter->GetActorForwardVector();
		PlayerForwardVector.Z = 0.0f;
		PlayerForwardVector.Normalize();
		
		float PlayerDotValue = FVector::DotProduct(PlayerForwardVector, DirectionToGimmick);
		float HalfFOVAngle = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(PlayerDotValue, -1.0f, 1.0f)));
	
		float CurrentFOVAngle = HalfFOVAngle * 2.0f;
		
		if (CurrentFOVAngle >= TargetFOVAngle)
		{
			PS3_BROADCAST_TO_MVVM_OneParams(OnTutorial_UI, true);
		}
	}
	
}


void US5_InteractionGimmickComponent::OnCharacterEndOverlapForUI(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (GetOwner() == nullptr) return;
	
	auto* PS3PlayerCharacter = Cast<APS3PlayerCharacter>(OtherActor);
	if (IsValid(PS3PlayerCharacter) == false) return;
	
	if (GetOwner()->HasAuthority() == true)
	{
		if (IsValid(OtherActor) == false) return;
		if (OverlappedCharacters.Contains(OtherActor) == false) return;
		OverlappedCharacters.Remove(OtherActor);
	}
	
	if (PS3PlayerCharacter->IsLocallyControlled() == true)
	{
		PS3_BROADCAST_TO_MVVM_OneParams(OnTutorial_UI, false);
	}
}


void US5_InteractionGimmickComponent::OnColletedGimmickBase(const UActorComponent* CurrentComponent, bool bIsInteractable)
{
	if (this == CurrentComponent)
	{
		bIsInteractionGimmick = bIsInteractable;
	}
}

