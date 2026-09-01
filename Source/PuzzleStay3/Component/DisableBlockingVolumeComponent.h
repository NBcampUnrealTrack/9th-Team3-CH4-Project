// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "DisableBlockingVolumeComponent.generated.h"

//플레이어가 오버랩 되면 모드한테 알림 -> 구현예정


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PUZZLESTAY3_API UDisableBlockingVolumeComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	UDisableBlockingVolumeComponent();

	UFUNCTION()
	bool IsOverlapped() const;

protected:
	virtual void BeginPlay() override;

private:
	bool bIsOverlapped = false;

	UFUNCTION()
	void OnOverlapDeathVolume(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
		);
};
