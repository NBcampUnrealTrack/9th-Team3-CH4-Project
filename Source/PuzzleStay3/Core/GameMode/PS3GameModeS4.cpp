// Fill out your copyright notice in the Description page of Project Settings.


#include "PS3GameModeS4.h"
#include "Data/Enum/DoorType.h"

void APS3GameModeS4::BeginPlay()
{
	Super::BeginPlay();

	const int32 RandomIndex = FMath::RandRange(0, SubstituteFixedObjectWeight.Num() - 1);

	FixedObjectWeight = SubstituteFixedObjectWeight[RandomIndex];
	
	// 저울 반대편에 올려야 하는 덤벨 합계
	TargetBalancedWeight = FixedObjectWeight - (Player1Weight + Player2Weight);

	UE_LOG(LogTemp, Warning, TEXT("Stage4 Fixed Weight: %.0f"), FixedObjectWeight);
	UE_LOG(LogTemp, Warning, TEXT("Stage4 Target Dumbbell Weight: %.0f"), TargetBalancedWeight);
}

//저울이 정답 판별 후 이 함수 호출
void APS3GameModeS4::NotifyJeoulResult(bool bIsSuccess)
{
	if (!bIsSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Stage4] Jeoul result: Wrong"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Stage4] Jeoul result: Correct"));

	OpenStage4FirstDoor();
}

void APS3GameModeS4::OpenStage4FirstDoor()
{
	//OnEscapeDoorOpened.Broadcast(EDoorType::Stage4FirstDoor);
	OnEscapeDoorOpened.Broadcast();
}
