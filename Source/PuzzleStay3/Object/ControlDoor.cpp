#include "ControlDoor.h"

#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Components/TimelineComponent.h"
#include "Core/GameMode/PS3GameModeS5.h"
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
	
	
	DecalComp_A = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp_A"));
	DecalComp_A->SetupAttachment(RootComponent);
	DecalComp_A->DecalSize = FVector(128.0f, 256.0f, 256.0f);
	DecalComp_A->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	DecalComp_A->SetVisibility(false);
	
	DecalComp_B = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp_B"));
	DecalComp_B->SetupAttachment(RootComponent);
	DecalComp_B->DecalSize = FVector(128.0f, 256.0f, 256.0f);
	DecalComp_B->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	DecalComp_B->SetVisibility(false);
	
	
	DecalComp_C = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp_C"));
	DecalComp_C->SetupAttachment(RootComponent);
	DecalComp_C->DecalSize = FVector(128.0f, 256.0f, 256.0f);
	DecalComp_C->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	DecalComp_C->SetVisibility(false);
	
	
	DecalComp_D = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp_D"));
	DecalComp_D->SetupAttachment(RootComponent);
	DecalComp_D->DecalSize = FVector(128.0f, 256.0f, 256.0f);
	DecalComp_D->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	DecalComp_D->SetVisibility(false);
}


void AControlDoor::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(DoorMesh) == true && IsValid(DoorTimelineComp) == true && IsValid(DoorTimeLineCurve) == true)
	{
		FOnTimelineFloat OnTimelineUpdate;
		OnTimelineUpdate.BindUFunction(this, FName("OnTimelineUpdate"));
		DoorTimelineComp->AddInterpFloat(DoorTimeLineCurve, OnTimelineUpdate);
		
		FOnTimelineEvent OnTimelineFinished;
		OnTimelineFinished.BindUFunction(this, FName("OnTimelineFinished"));
		DoorTimelineComp->SetTimelineFinishedFunc(OnTimelineFinished);

		StartLocation = DoorMesh->GetRelativeLocation();
	}
	
	if (HasAuthority() == true)
	{
		auto* PS3GameModeS5 = Cast<APS3GameModeS5>(GetWorld()->GetAuthGameMode());
		if (IsValid(PS3GameModeS5) == false) return;
		
		PS3GameModeS5->OnScreenPlayerSpawned.AddUObject(this, &ThisClass::OnScreenPlayerSpawned);
	}
	
	SetVisibleDecalToDoorType();
}


void AControlDoor::SetVisibleDecalToDoorType()
{
	if (S5_DoorType == EControlDoorType::Door_A)
	{
		DecalComp_A->SetVisibility(true);
	}
	else if (S5_DoorType == EControlDoorType::Door_B)
	{
		DecalComp_B->SetVisibility(true);
	}
	else if (S5_DoorType == EControlDoorType::Door_C)
	{
		DecalComp_C->SetVisibility(true);
	}
	else if (S5_DoorType == EControlDoorType::Door_D)
	{
		DecalComp_D->SetVisibility(true);
	}
}


void AControlDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsEscapeDoorOpen);
	DOREPLIFETIME(ThisClass, bIsDoorOpen);
	DOREPLIFETIME(ThisClass, bIsScreenPlayerSpawned);
	
}


void AControlDoor::NetMulti_OnOperateDoor_Implementation(EControlDoorType PressedButtonType , bool PressedType)
{
	if (bIsScreenPlayerSpawned == true) return;
	
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

void AControlDoor::OnScreenPlayerSpawned()
{
	if (HasAuthority() == true)
	{
		bIsDoorOpen = true;
		bIsScreenPlayerSpawned = true;
		NetMultiRPC_OnScreenPlayerSpawned();
	}
}


void AControlDoor::NetMultiRPC_OnScreenPlayerSpawned_Implementation()
{
	if (IsValid(DoorTimelineComp) == false) return;
	DoorTimelineComp->Play();
}




