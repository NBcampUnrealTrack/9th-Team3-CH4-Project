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
class UAnimMontage;

DECLARE_MULTICAST_DELEGATE(FOnJeoulCheckStarted);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnJeoulCheckFinished, bool);

UENUM(BlueprintType)
enum class EJeoulState : uint8
{
	Idle,     // 대기 상태
	Checking, // 컷씬 및 기울기 연출 진행 중
	Resolved  // 정답 완료
};

UENUM(BlueprintType)
enum class EJeoulTiltState : uint8
{
	Balanced, // 수평
	TiltLeft, // 왼쪽 기울음
	TiltRight // 오른쪽 기울음
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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#pragma region Mesh & Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> JeoulSkeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> OverlapTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> PlateTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> CutsceneCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Spots")
	TObjectPtr<USceneComponent> Player1Spot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Spots")
	TObjectPtr<USceneComponent> Player2Spot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Spots")
	TObjectPtr<USkeletalMeshComponent> DummyPlayer1Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Spots")
	TObjectPtr<USkeletalMeshComponent> DummyPlayer2Mesh;
	
	UPROPERTY(EditAnywhere, Category = "Jeoul Settings|Cutscene")
	float DummyYawOffset = 0.0f;
	
	UPROPERTY(EditAnywhere, Category = "Jeoul Settings|Cutscene")
	FName HandSocketName = TEXT("hand_rSocket");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Spots")
	TArray<TObjectPtr<USceneComponent>> DumbbellSpots;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Jeoul Settings")
	TObjectPtr<AActor> ExternalSwitchActor;

	UPROPERTY(EditAnywhere, Category = "Jeoul Settings|Animation")
	TObjectPtr<UAnimMontage> LeftTiltMontage;

	UPROPERTY(EditAnywhere, Category = "Jeoul Settings|Animation")
	TObjectPtr<UAnimMontage> RightTiltMontage;

private:
	UPROPERTY()
	TObjectPtr<UInteractionSwitchComponent> ExternalSwitch;

#pragma endregion

public:
	UCameraComponent* GetCutsceneCamera() const { return CutsceneCamera; }

	UFUNCTION(BlueprintCallable, Category = "Jeoul")
	EJeoulTiltState GetTiltState() const { return CurrentTiltState; }

	UFUNCTION(BlueprintCallable, Category = "Jeoul")
	EJeoulState GetJeoulState() const { return CurrentState; }

	float CalculateWeightOnPlate(UBoxComponent* InPlateTrigger);

	UFUNCTION(Server, Reliable)
	void Server_CheckBalance();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnJeoulCheckStarted();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnJeoulCheckFinished(bool bIsSuccess);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayTiltAnimation(EJeoulTiltState TiltState);

	void RequestCutsceneReturn(APS3PlayerController* RequestingController);
	void AlignPlayersAndDumbbells();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RestorePlayerCharacter(APS3PlayerCharacter* TargetCharacter);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AlignPlayerCharacter(APS3PlayerCharacter* TargetCharacter, FVector TargetLocation,
	                                    FRotator TargetRotation, USceneComponent* AttachTarget);

private:
	void OnCheckButtonPressed(bool bActivated);
	bool HasBothPlayersOnPlate() const;

	static void SetupBlockingMesh(UPrimitiveComponent* Mesh, ECollisionResponse VisibilityResponse);

	UPROPERTY(Replicated)
	EJeoulState CurrentState = EJeoulState::Idle;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EJeoulTiltState CurrentTiltState = EJeoulTiltState::Balanced;

	UPROPERTY(EditAnywhere, Category = "Jeoul Settings")
	float CutSceneTime = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Jeoul Settings")
	float ResetBeamTime = 1.5f;

	TArray<TWeakObjectPtr<APS3PlayerController>> CutsceneParticipants;
};