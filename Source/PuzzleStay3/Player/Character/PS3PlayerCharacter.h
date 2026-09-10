
#pragma once

#include "Data/Enum/PS3PlayerIdentity.h"
#include "GameFramework/Character.h"
#include "PS3PlayerCharacter.generated.h"

class UUW_HPText;
class UDXHPTextWidgetComponent;
class UDXStatusComponent;
class UCameraComponent;
class USpringArmComponent;
class ADumbbell;
class USceneComponent;

UCLASS()
class PUZZLESTAY3_API APS3PlayerCharacter : public ACharacter
{
	GENERATED_BODY()


public:
	APS3PlayerCharacter();

	UFUNCTION(BlueprintPure, Category = "PS3|Character")
	bool CanUseFieldControls() const;
	
	UFUNCTION(BlueprintPure, Category = "PS3|Character|Interaction")
	USceneComponent* GetCarryAnchor() const
	{
		return CarryAnchor;
	}

	UFUNCTION(BlueprintPure, Category = "PS3|Character|Interaction")
	ADumbbell* GetHeldDumbbell() const { return HeldDumbbell; }

	void Move(const FVector2D& InMovementVector);
	void Look(const FVector2D& InLookVector);
	void StartJump();
	void StopJump();
	void TryInteract();
	void TryDropHeldObject();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "PS3|Character|Respawn")
	void PrepareForRespawn();

	// PlayerState가 준비되거나 P1/P2 식별값이 변경됐을 때 외형을 다시 적용
	void RefreshPlayerIdentityVisual();
	

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	// 실제 메시/Cosmetic 적용은 Character Blueprint에서 구현
	UFUNCTION(BlueprintImplementableEvent, Category = "PS3|Character|Visual", meta = (DisplayName = "Apply Player Identity Visual"))
	void BP_ApplyPlayerIdentityVisual(EPS3PlayerIdentity PlayerIdentity);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PS3|Character|Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PS3|Character|Components")
	TObjectPtr<UCameraComponent> Camera;

	//상호작용할 라인트레이스 거리
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS3|Character|Interaction")
	float InteractionDistance = 300.0f;

	// 에디터와 개발 빌드에서 서버 판정 라인트레이스를 로컬 플레이어 화면에 표시
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS3|Character|Interaction|Debug")
	bool bDrawInteractionTrace = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS3|Character|Interaction|Debug", meta = (ClampMin = "0.0"))
	float InteractionTraceDebugDuration = 2.0f;

	//덤벨 held, drop을 위한 변수
	UPROPERTY(Transient)
	TObjectPtr<ADumbbell> HeldDumbbell;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "PS3|Character|Interaction")
	TObjectPtr<USceneComponent> CarryAnchor;

private:
	UFUNCTION(Server, Reliable)
	void Server_TryInteract();

	UFUNCTION(Client, Unreliable)
	void Client_DrawInteractionTrace(FVector TraceStart, FVector TraceEnd, bool bHit, FVector ImpactPoint);

	UFUNCTION(Server, Reliable)
	void Server_TryDropHeldObject();

	UObject* FindInteractableTarget(AActor* HitActor) const;
};
