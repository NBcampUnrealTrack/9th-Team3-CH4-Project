#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Player/Interaction/PS3InteractableInterface.h"
#include "Jeoul.generated.h"

class APS3PlayerCharacter;
class APS3PlayerController;
class UInteractionSwitchComponent;
class UCameraComponent;
class ADumbbell;
class UBoxComponent;

DECLARE_MULTICAST_DELEGATE(FOnJeoulCheckStarted);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnJeoulCheckFinished, bool);

UENUM(BlueprintType)
enum class EJeoulState : uint8
{
	Idle, // 대기 상태
	Checking, // 컷씬 및 기울기 연출 진행 중
	Resolved // 정답 완료
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> JeoulBaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> JeoulBeamMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> BeamPivot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> PlateTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> CutsceneCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInteractionSwitchComponent> InteractionSwitchComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> CheckButtonMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Spots")
	TObjectPtr<USceneComponent> Player1Spot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Spots")
	TObjectPtr<USceneComponent> Player2Spot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Spots")
	TArray<TObjectPtr<USceneComponent>> DumbbellSpots;

#pragma endregion

public:
	UCameraComponent* GetCutsceneCamera() const { return CutsceneCamera; }

	float CalculateWeightOnPlate(UBoxComponent* InPlateTrigger);

	UFUNCTION(Server, Reliable)
	void Server_CheckBalance();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnJeoulCheckStarted();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnJeoulCheckFinished(bool bIsSuccess);

	void RequestCutsceneReturn(APS3PlayerController* RequestingController);
	void AlignPlayersAndDumbbells();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RestorePlayerCharacter(APS3PlayerCharacter* TargetCharacter);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AlignPlayerCharacter(APS3PlayerCharacter* TargetCharacter, FVector TargetLocation,
	                                    FRotator TargetRotation, USceneComponent* AttachTarget);

private:
	void OnCheckButtonPressed(bool bActivated);

	UFUNCTION()
	void OnRep_TargetBeamRotation();

	bool HasBothPlayersOnPlate() const;

	UPROPERTY(EditAnywhere, Category = "Jeoul Settings")
	float MaxTiltAngle = 25.0f;

	UPROPERTY(EditAnywhere, Category = "Jeoul Settings")
	float TiltSensitivity = 10.0f;

	FRotator InitialBeamRotation;

	UPROPERTY(ReplicatedUsing = OnRep_TargetBeamRotation)
	FRotator TargetBeamRotation;

	static void SetupBlockingMesh(UStaticMeshComponent* Mesh, ECollisionResponse VisibilityResponse);

	UPROPERTY(Replicated)
	EJeoulState CurrentState = EJeoulState::Idle;

	UPROPERTY(EditAnywhere, Category = "Jeoul Settings")
	float CutSceneTime = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Jeoul Settings")
	float ResetBeamTime = 1.5f;

	TArray<TWeakObjectPtr<APS3PlayerController>> CutsceneParticipants;
};
