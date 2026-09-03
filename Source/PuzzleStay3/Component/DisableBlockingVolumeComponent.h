// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "DisableBlockingVolumeComponent.generated.h"

//Stage1에서 Player1이 오버랩 시 Stage1의 블록볼륨 콜리전을 해제하기 위한 컴포넌트

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PUZZLESTAY3_API UDisableBlockingVolumeComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	UDisableBlockingVolumeComponent();

protected:
	virtual void BeginPlay() override;

private:
	bool bIsOverlapped = false;

	UFUNCTION()
	void OnOverlapDisableBlockingVolume(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
		);
};
