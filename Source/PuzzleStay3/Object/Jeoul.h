#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Jeoul.generated.h"

class ADumbbell;
class UBoxComponent;
class UInteractionSwitchComponent;



UCLASS()
class PUZZLESTAY3_API AJeoul : public AActor
{
	GENERATED_BODY()
	
public:	
	AJeoul();

protected:
	virtual void BeginPlay() override;

#pragma region Mesh
	// 컴포넌트 구성
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;
	
	// 저울 몸체/기둥
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> JeoulBaseMesh;
	
	// 기울어지는 저울대 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> JeoulBeamMesh;
	
	// 좌/우 저울판 감지용 Trigger Box
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> LeftPlateTrigger;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> RightPlateTrigger;
	
	// 확인 버튼용 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInteractionSwitchComponent> InteractionSwitchComp;
	
#pragma endregion 

public:
	// 좌/우 저울판 위 액터들의 무게 합산
	float CalculateWeightOnPlate(UBoxComponent* PlateTrigger) const;
	
	// 서버 권한 균형 검증 RPC (확인 버튼 상호작용 시 호출)
	UFUNCTION(Server, Reliable)
	void Server_CheckBalance();
	
private:
	// 스위치 상호작용 콜백
	void OnCheckButtonPressed(bool bActivated);

	// 실패 시 저울대의 기울기(회전) 원위치 리셋
	void ResetJeoulRotation();
	
	// 저울대 초기 회전값
	FRotator InitialBeamRotation;
	
	// 플레이어 무게 설정값 (Player A = 1.0f, Player B = 0.5f)
	UPROPERTY(EditAnywhere, Category = "Jeoul Settings")
	float PlayerAWeight = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Jeoul Settings")
	float PlayerBWeight = 0.5f;
	
};
