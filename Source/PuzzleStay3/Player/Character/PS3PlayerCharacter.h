
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

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "PS3|Character")
	void SetCanUseFieldControls(bool bEnable);
	
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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	UPROPERTY(Replicated)
	bool bCanUseFieldControls = true;

	// 실제 메시/Cosmetic 적용은 Character Blueprint에서 구현
	UFUNCTION(BlueprintImplementableEvent, Category = "PS3|Character|Visual", meta = (DisplayName = "Apply Player Identity Visual"))
	void BP_ApplyPlayerIdentityVisual(EPS3PlayerIdentity PlayerIdentity);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PS3|Character|Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PS3|Character|Components")
	TObjectPtr<UCameraComponent> Camera;

	// 플레이어 전방에 생성할 상호작용 구체의 반경
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS3|Character|Interaction", meta = (ClampMin = "0.0"))
	float InteractionSphereRadius = 70.0f;

	// 플레이어 캡슐 앞면과 상호작용 구체 표면 사이의 거리
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS3|Character|Interaction")
	float InteractionSphereForwardOffset = -20.0f;
	
	// 플레이어 중심을 기준으로 한 구체 높이
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS3|Character|Interaction", meta = (ClampMin = "0.0"))
	float InteractionSphereHeight = 0.0f;

	// 에디터와 개발 빌드에서 서버 판정 구체 범위를 로컬 플레이어 화면에 표시
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
	void Client_DrawInteractionTrace(FVector SphereCenter, bool bHit, FVector ImpactPoint);

	UFUNCTION(Server, Reliable)
	void Server_TryDropHeldObject();

	UObject* FindInteractableTarget(AActor* HitActor) const;
};
