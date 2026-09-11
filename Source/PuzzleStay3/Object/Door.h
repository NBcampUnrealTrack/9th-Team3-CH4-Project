#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/Enum/DoorType.h"
#include "Door.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsDoorOpen, bool);

class UOverlapSwitchComponent;
class UInteractionSwitchComponent;

UCLASS()
class PUZZLESTAY3_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADoor();
	
	FOnIsDoorOpen OnIsDoorOpen;
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
#pragma region Mesh Component & Settings
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DoorMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Settings")
	EDoorType DoorType = EDoorType::StageAllFinalDoor;
	
	// 문 식별 ID (스위치의 SwitchID와 동일한 것만 연동)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Settings")
	int32 DoorID = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Movement")
	FVector TargetRelativeLocation = FVector(0.f, 0.f, -250.f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Movement")
	float OpenSpeed = 2.0f;
	
#pragma endregion 
	
#pragma region ID-Based Direct Binding
protected:
	// 월드 내 SwitchID가 내 DoorID와 일치하는 모든 스위치 탐색 및 자동 바인딩
	void BindSwitchesByID();

	// 연결된 스위치들의 활성화 상태를 검사하여 문 열림/닫힘 판단
	void EvaluateDoorState();

	UFUNCTION()
	void OnLinkedSwitchStateChanged(bool bIsActivated);

private:
	// 내 DoorID와 매칭된 오버랩 스위치 컴포넌트 목록
	UPROPERTY()
	TArray<TObjectPtr<UOverlapSwitchComponent>> LinkedOverlapSwitches;

	// 내 DoorID와 매칭된 인터렉션 스위치 컴포넌트 목록
	UPROPERTY()
	TArray<TObjectPtr<UInteractionSwitchComponent>> LinkedInteractionSwitches;
	
#pragma endregion
	
#pragma region Door State & Callbacks
protected:	
	UPROPERTY(ReplicatedUsing = OnRep_bIsOpen)
	bool bIsOpen = false;
	
	UFUNCTION()
	void OnRep_bIsOpen();
	
	// 단일 bool 전달형 문 열림 콜백 (최종 탈출문, Stage4 등)
	UFUNCTION()
	void OnOpenDoor(bool bOpened);
	
#pragma endregion

private:
	FVector InitialRelativeLocation;
	
};
