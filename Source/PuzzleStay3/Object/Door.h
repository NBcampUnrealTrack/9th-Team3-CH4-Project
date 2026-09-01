#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	
	// 문이 열릴 때 이동할 상대 위치 (에디터에서 설정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Movement")
	FVector TargetRelativeLocation = FVector(0.f, 0.f, 300.f);
	
	// 문 열림 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Movement")
	float OpenSpeed = 2.0f;
	
#pragma endregion 
	
protected:
	// GameMode 델리게이트 수신 시 호출될 함수
	UFUNCTION()
	void OnOpenDoor();
	
	UPROPERTY(ReplicatedUsing = OnRep_bIsOpen)
	bool bIsOpen = false;
	
	UFUNCTION()
	void OnRep_bIsOpen();
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	FVector InitialRelativeLocation;
	
};
