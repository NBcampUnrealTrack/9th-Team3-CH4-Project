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

#pragma region Mesh
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DoorMesh;
	
	// 문 타입 선택 (에디터 디테일 창에서 설정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Settings")
	EDoorType DoorType = EDoorType::StageAllFinalDoor;
	
	// 문이 열릴 때 이동할 상대 위치 (에디터에서 설정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Movement")
	FVector TargetRelativeLocation = FVector(0.f, 0.f, 300.f);
	
	// 문 열림 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Movement")
	float OpenSpeed = 2.0f;
	
#pragma endregion 
	
#pragma region Direct Test Setup
protected:
	// ★ [테스트용] 에디터 디테일 창에서 이 문과 직접 연결할 발판 액터를 지정합니다.
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Door|TestBinding")
	TObjectPtr<AActor> TargetPressurePlateActor;

	// ★ [테스트용] 발판의 OverlapSwitchComponent 델리게이트를 직접 수신하는 콜백
	UFUNCTION()
	void OnDirectOverlapStateChanged(bool bIsOverlapped);
#pragma endregion
	
protected:	
	UPROPERTY(ReplicatedUsing = OnRep_bIsOpen)
	bool bIsOpen = false;
	
	UFUNCTION()
	void OnRep_bIsOpen();
	
	// GameState의 OnEscapeDoorOpened 델리게이트 수신 콜백 (bool bOpened 매개변수 추가)
	UFUNCTION()
	void OnOpenDoor(bool bOpened);
	
	// Stage 1 등 식별이 필요한 문 ID (에디터 디테일 창에서 설정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Settings")
	int32 DoorID = 1;

	// Stage1 문 상태 수신 콜백
	UFUNCTION()
	void OnStage1DoorStateChanged(int32 InDoorID, bool bOpened);
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	FVector InitialRelativeLocation;
	
};
