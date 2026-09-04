
#pragma once

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

	void Move(const FVector2D& InMovementVector);
	void Look(const FVector2D& InLookVector);
	void StartJump();
	void StopJump();
	void TryInteract();
	void TryDropHeldObject();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "PS3|Character|Respawn")
	void PrepareForRespawn();
	

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PS3|Character|Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PS3|Character|Components")
	TObjectPtr<UCameraComponent> Camera;

	//상호작용할 라인트레이스 거리
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS3|Character|Interaction")
	float InteractionDistance = 300.0f;

	//덤벨 held, drop을 위한 변수
	UPROPERTY(Transient)
	TObjectPtr<ADumbbell> HeldDumbbell;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "PS3|Character|Interaction")
	TObjectPtr<USceneComponent> CarryAnchor;
	
private:
	UFUNCTION(Server, Reliable)
	void Server_TryInteract();

	UFUNCTION(Server, Reliable)
	void Server_TryDropHeldObject();
};
