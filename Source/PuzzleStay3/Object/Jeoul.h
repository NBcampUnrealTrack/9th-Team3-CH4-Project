#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Player/Interaction/PS3InteractableInterface.h"
#include "Jeoul.generated.h"

class UInteractionSwitchComponent;
class UCameraComponent;
class ADumbbell;
class UBoxComponent;

DECLARE_MULTICAST_DELEGATE(FOnJeoulCheckStarted);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnJeoulCheckFinished, bool);

UENUM(BlueprintType)
enum class EJeoulState : uint8
{
	Idle,			// 대기 상태
	Checking,		// 컷씬 및 기울기 연출 진행 중
	Resolved		// 정답 완료
};

UCLASS()
class PUZZLESTAY3_API AJeoul : public AActor
{
	GENERATED_BODY()
	
public:	
	AJeoul();
	FOnJeoulCheckStarted OnJeoulCheckStarted;
	FOnJeoulCheckFinished OnJeoulCheckFinished;
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
#pragma region Mesh & Components
	// 컴포넌트 구성
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;
	
	// 저울 몸체/기둥
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> JeoulBaseMesh;
	
	// 기울어지는 저울대 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> JeoulBeamMesh;
	
	// 회전 축을 담당할 피벗 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> BeamPivot;
	
	// 저울판 감지용 Trigger Box
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> PlateTrigger;
	
	// 3인칭 -> 컷씬 전환용 저울 전경 카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> CutsceneCamera;
	
	// 확인 버튼용 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInteractionSwitchComponent> InteractionSwitchComp;

	// 플레이어가 F키로 바라보고 누를 버튼 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> CheckButtonMesh;
	
#pragma endregion 

public:
	// 컷씬 카메라 컴포넌트 Getter
	UCameraComponent* GetCutsceneCamera() const { return CutsceneCamera; }
	
	// 좌/우 저울판 위 액터들의 무게 합산
	float CalculateWeightOnPlate(UBoxComponent* InPlateTrigger);
	
	// 서버 권한 균형 검증 RPC (확인 버튼 상호작용 시 호출)
	UFUNCTION(Server, Reliable)
	void Server_CheckBalance();
	
	// 클라이언트 전체에 연출 시작 알림
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnJeoulCheckStarted();

	// 클라이언트 전체에 연출 완료/결과 알림
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnJeoulCheckFinished(bool bIsSuccess);
	
private:
	// 스위치 상호작용 콜백
	void OnCheckButtonPressed(bool bActivated);
	
	UFUNCTION()
	void OnRep_TargetBeamRotation();
	
	// 플레이어 2명이 저울판 위에 있는지 확인
	bool HasBothPlayersOnPlate() const;
	
	// 최대 기울기 각도 (예: 25도)
	UPROPERTY(EditAnywhere, Category = "Jeoul Settings")
	float MaxTiltAngle = 25.0f;
	
	// 무게 1 차이당 기울어질 각도 비율
	UPROPERTY(EditAnywhere, Category = "Jeoul Settings")
	float TiltSensitivity = 10.0f;
	
	// 수평 상태 기본 저울대 초기 회전값
	FRotator InitialBeamRotation;

	// 목표 회전값 (Tick에서 부드럽게 보간)
	UPROPERTY(ReplicatedUsing = OnRep_TargetBeamRotation)
	FRotator TargetBeamRotation;

	static void SetupBlockingMesh(UStaticMeshComponent* Mesh, ECollisionResponse VisibilityResponse);
	
	// 저울 상태
	UPROPERTY(Replicated)
	EJeoulState CurrentState = EJeoulState::Idle;
	
	// 컷씬 및 기울기 연출 대기 시간 (기본값: 3초)
	UPROPERTY(EditAnywhere, Category = "Jeoul Settings")
	float CutSceneTime = 3.0f;

	// 실패 시 저울대가 수평으로 복구되는 연출 대기 시간 (기본값: 1.5초)
	UPROPERTY(EditAnywhere, Category = "Jeoul Settings")
	float ResetBeamTime = 1.5f;
};
