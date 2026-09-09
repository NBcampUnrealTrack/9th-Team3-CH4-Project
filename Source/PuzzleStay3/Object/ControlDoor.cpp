#include "ControlDoor.h"

#include "EngineUtils.h"
#include "Algo/RandomShuffle.h"
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

	ControlDoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	ControlDoorMesh->SetupAttachment(RootComponent);
	
	BlockingVolumeComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BlockingVolumeComp"));
	BlockingVolumeComp->SetupAttachment(RootComponent);
	
	DoorTimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("DoorTimeLineComp"));
	DoorTimelineComp->SetPropertySetObject(this);
	
	
	DecalComp_A = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp_A"));
	DecalComp_A->SetupAttachment(ControlDoorMesh);
	DecalComp_A->DecalSize = FVector(128.0f, 256.0f, 256.0f);
	DecalComp_A->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	DecalComp_A->SetVisibility(true);
	
	DecalComp_B = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp_B"));
	DecalComp_B->SetupAttachment(ControlDoorMesh);
	DecalComp_B->DecalSize = FVector(128.0f, 256.0f, 256.0f);
	DecalComp_B->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	DecalComp_B->SetVisibility(true);
	
	
	DecalComp_C = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp_C"));
	DecalComp_C->SetupAttachment(ControlDoorMesh);
	DecalComp_C->DecalSize = FVector(128.0f, 256.0f, 256.0f);
	DecalComp_C->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	DecalComp_C->SetVisibility(true);
	
	
	DecalComp_D = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp_D"));
	DecalComp_D->SetupAttachment(ControlDoorMesh);
	DecalComp_D->DecalSize = FVector(128.0f, 256.0f, 256.0f);
	DecalComp_D->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	DecalComp_D->SetVisibility(true);
}


void AControlDoor::BeginPlay()
{
	Super::BeginPlay();
	
	ErrorCheck_S5();
	InitializeRandomControlDoorType();
	InitializeBindFunction();
	TimeLineCurveBind();
	
	SetVisibleDecalToDoorType();
}


void AControlDoor::SetVisibleDecalToDoorType()
{
	if (IsValid(DecalComp_A) == true) DecalComp_A->SetVisibility(false);
	if (IsValid(DecalComp_B) == true) DecalComp_B->SetVisibility(false);
	if (IsValid(DecalComp_C) == true) DecalComp_C->SetVisibility(false);
	if (IsValid(DecalComp_D) == true) DecalComp_D->SetVisibility(false);
	
	if (ControlDoorType == EControlDoorType::Door_A)
	{
		DecalComp_A->SetVisibility(true);
	}
	else if (ControlDoorType == EControlDoorType::Door_B)
	{
		DecalComp_B->SetVisibility(true);
	}
	else if (ControlDoorType == EControlDoorType::Door_C)
	{
		DecalComp_C->SetVisibility(true);
	}
	else if (ControlDoorType == EControlDoorType::Door_D)
	{
		DecalComp_D->SetVisibility(true);
	}
}


void AControlDoor::OnRep_ControlDoorType()
{
	SetVisibleDecalToDoorType();
}


void AControlDoor::OnGameStart(bool CurrentGameState)
{
	if (HasAuthority() == true)
	{
		if (CurrentGameState == false) return;
	
		bIsGameStart = CurrentGameState;
	}
}


void AControlDoor::InitializeRandomControlDoorType()
{
	if (HasAuthority() == false) return;
	
	if (ControlDoorType == EControlDoorType::Random)
	{
		TArray<EControlDoorType>  ShuffledControlDoorTypeArray = 
		{
			EControlDoorType::Door_A,
			EControlDoorType::Door_B,
			EControlDoorType::Door_C,
			EControlDoorType::Door_D
		};

		Algo::RandomShuffle( ShuffledControlDoorTypeArray);
		
		//이렇게 할 수도 있다.
		//int32 RandomValue = FMath::Floor(FMath::FRandRange(0.0, 3.0));
		
		ControlDoorType =  ShuffledControlDoorTypeArray[0];
	}
}


void AControlDoor::InitializeBindFunction()
{
	if (HasAuthority() == true)
	{
		auto* PS3GameModeS5 = Cast<APS3GameModeS5>(GetWorld()->GetAuthGameMode());
		if (IsValid(PS3GameModeS5) == false) return;
		
		PS3GameModeS5->OnIsGameStart.AddUObject(this, &ThisClass::OnGameStart);
		PS3GameModeS5->OnScreenPlayerSpawned.AddUObject(this, &ThisClass::OnScreenPlayerSpawned);
	}
}


void AControlDoor::TimeLineCurveBind()
{
	if (IsValid(ControlDoorMesh) == true && IsValid(DoorTimelineComp) == true && IsValid(DoorTimeLineCurve) == true)
	{
		FOnTimelineFloat OnTimelineUpdate;
		OnTimelineUpdate.BindUFunction(this, FName("OnTimelineUpdate"));
		DoorTimelineComp->AddInterpFloat(DoorTimeLineCurve, OnTimelineUpdate);
		
		FOnTimelineEvent OnTimelineFinished;
		OnTimelineFinished.BindUFunction(this, FName("OnTimelineFinished"));
		DoorTimelineComp->SetTimelineFinishedFunc(OnTimelineFinished);

		StartLocation = ControlDoorMesh->GetRelativeLocation();
	}
}


void AControlDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ControlDoorType);
	DOREPLIFETIME(ThisClass, bIsGameStart);
	DOREPLIFETIME(ThisClass, bIsEscapeDoorOpen);
	DOREPLIFETIME(ThisClass, bIsDoorOpen);
	DOREPLIFETIME(ThisClass, bIsPressed);
	DOREPLIFETIME(ThisClass, bIsScreenPlayerCharacterSpawned);
	
}


void AControlDoor::NetMulti_OnOperateDoor_Implementation(EControlDoorType PressedButtonType , bool bIsOpen)
{
	if (bIsGameStart == false) return;
	if (bIsScreenPlayerCharacterSpawned == true) return;
	
	bIsDoorOpen = bIsOpen;
	
	if (PressedButtonType == ControlDoorType && IsValid(DoorTimelineComp) == true)
	{
		if (IsValid(BlockingVolumeComp) == true)
		{
			BlockingVolumeComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			BlockingVolumeComp->SetCollisionResponseToAllChannels(ECR_Block);
			
			UE_LOG(LogTemp, Warning, TEXT("문이 열리지않았거나 열리는중, 통행 불가능"));
		}
		
		if (bIsDoorOpen == true)
		{
			if (DoorTimelineComp->IsPlaying() == false && IsValid(ControlDoorMesh))
			{
				StartLocation = ControlDoorMesh->GetRelativeLocation();
			}
			
			OnIsControlDoorOpen.Broadcast(true);
			DoorTimelineComp->Play();
		}
		
		else
		{
			OnIsControlDoorOpen.Broadcast(false);
			DoorTimelineComp->Reverse();
		}
	}
}


void AControlDoor::OnTimelineUpdate(float Value)
{
	if (IsValid(ControlDoorMesh) == false) return;
	
	FVector DistanceToMovement = StartLocation + TargetLocation;
	FVector MovementLocation = FMath::Lerp(StartLocation, DistanceToMovement, Value);

	ControlDoorMesh->SetRelativeLocation(MovementLocation);
	
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
		bIsScreenPlayerCharacterSpawned = true;
		NetMultiRPC_OnScreenPlayerSpawned();
	}
}


void AControlDoor::NetMultiRPC_OnScreenPlayerSpawned_Implementation()
{
	if (IsValid(DoorTimelineComp) == false) return;
	DoorTimelineComp->Play();
}


void AControlDoor::ErrorCheck_S5()
{
	if (HasAuthority() == false) return;
	
	int32 TotalDoorCount = 0;
	int32 InvalidDoorCount = 0;

	for (TActorIterator<AControlDoor> It(GetWorld()); It; ++It)
	{
		AControlDoor* Door = *It;
		if (IsValid(Door) == true)
		{
			TotalDoorCount++;
			
			if (Door->ControlDoorType == EControlDoorType::None)
			{
				InvalidDoorCount++;
			}
		}
	}

	checkf(InvalidDoorCount == 0, 
		TEXT("Type 선정 오류: 전체 %d 개의 [AControlDoor] 중 Type이 <None>으로 선정 된 액터가 %d개 있습니다. Type을 선정해주세요."), 
		TotalDoorCount, InvalidDoorCount);
}


