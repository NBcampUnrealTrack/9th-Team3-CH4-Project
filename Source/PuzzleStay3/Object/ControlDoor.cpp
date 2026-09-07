#include "ControlDoor.h"

#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Net/UnrealNetwork.h"


AControlDoor::AControlDoor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(RootComponent);
	
	BlockingVolumeComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BlockingVolumeComp"));
	BlockingVolumeComp->SetupAttachment(RootComponent);
	
	DoorTimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("DoorTimeLineComp"));
	DoorTimelineComp->SetPropertySetObject(this);
}


void AControlDoor::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(DoorMesh) == false) return;
	if (IsValid(DoorTimelineComp) == true && IsValid(DoorTimeLineCurve) == true)
	{
		FOnTimelineFloat OnTimelineUpdate;
		OnTimelineUpdate.BindUFunction(this, FName("OnTimelineUpdate"));
		DoorTimelineComp->AddInterpFloat(DoorTimeLineCurve, OnTimelineUpdate);
		
		FOnTimelineEvent OnTimelineFinished;
		OnTimelineFinished.BindUFunction(this, FName("OnTimelineFinished"));
		DoorTimelineComp->SetTimelineFinishedFunc(OnTimelineFinished);

		StartLocation = DoorMesh->GetRelativeLocation();
	}
}


void AControlDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsEscapeDoorOpen);
}


void AControlDoor::NetMulti_OnOperateDoor_Implementation(EControlDoorType PressedButtonType , bool PressedType)
{
	bIsDoorOpen = PressedType;
	
	if (PressedButtonType == S5_DoorType && IsValid(DoorTimelineComp) == true)
	{
		if (IsValid(BlockingVolumeComp) == true)
		{
			BlockingVolumeComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			BlockingVolumeComp->SetCollisionResponseToAllChannels(ECR_Block);
			
			UE_LOG(LogTemp, Warning, TEXT("문이 열리지않았거나 열리는중, 통행 불가능"));
		}
		
		if (bIsDoorOpen == true)
		{
			if (DoorTimelineComp->IsPlaying() == false && IsValid(DoorMesh))
			{
				StartLocation = DoorMesh->GetRelativeLocation();
			}
			
			DoorTimelineComp->Play();
		}
		
		else
		{
			DoorTimelineComp->Reverse();
		}
	}
}


void AControlDoor::OnTimelineUpdate(float Value)
{
	if (IsValid(DoorMesh) == false) return;
	
	FVector DistanceToMovement = StartLocation + TargetLocation;
	FVector MovementLocation = FMath::Lerp(StartLocation, DistanceToMovement, Value);

	DoorMesh->SetRelativeLocation(MovementLocation);
	
}


void AControlDoor::OnTimelineFinished()
{
	if (IsValid(BlockingVolumeComp) == false) return;
	
	if (bIsDoorOpen == true || bIsEscapeDoorOpen == true)
	{
		BlockingVolumeComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		UE_LOG(LogTemp, Warning, TEXT("문이 열림, 통행 가능"));
	}
	else
	{
		BlockingVolumeComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		BlockingVolumeComp->SetCollisionResponseToAllChannels(ECR_Block);
		UE_LOG(LogTemp, Warning, TEXT("문이 열리지않았거나 열리는중, 통행 불가능"));
	}
	
}



