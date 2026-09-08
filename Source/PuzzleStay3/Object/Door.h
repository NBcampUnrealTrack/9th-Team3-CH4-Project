#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/Enum/DoorType.h"
#include "Door.generated.h"

UCLASS()
class PUZZLESTAY3_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADoor();

#pragma region Mesh Component & Settings
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DoorMesh;
	
	// 문 타입 선택 (에디터 디테일 창에서 설정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Settings")
	EDoorType DoorType = EDoorType::StageAllFinalDoor;
	
	// Stage 1 등 ID 식별이 필요한 문 번호 (에디터 디테일 창에서 설정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Settings")
	int32 DoorID = 1;
	
	// 문이 열릴 때 이동할 상대 위치 (에디터에서 설정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Movement")
	FVector TargetRelativeLocation = FVector(0.f, 0.f, 250.f);
	
	// 문 열림 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Movement")
	float OpenSpeed = 1.0f;
	
#pragma endregion 
	
#pragma region Direct Test Binding (테스트 전용)
protected:
	// TODO: [테스트용] GameState/GameMode 없이 직접 발판 액터와 연결할 때 사용 (나중에 미사용 시 주석 처리)
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Door|TestBinding")
	TObjectPtr<AActor> TargetPressurePlateActor;

	// TODO: [테스트용] 직통 발판 오버랩 수신 콜백 (나중에 미사용 시 주석 처리)
	UFUNCTION()
	void OnDirectOverlapStateChanged(bool bIsOverlapped);
	
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

	// ID 식별형 문 열림 콜백 (Stage 1 등)
	UFUNCTION()
	void OnStage1DoorStateChanged(int32 InDoorID, bool bOpened);
	
#pragma endregion
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	FVector InitialRelativeLocation;
	
};
