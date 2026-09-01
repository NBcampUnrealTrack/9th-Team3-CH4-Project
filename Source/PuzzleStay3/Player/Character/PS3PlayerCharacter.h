
#pragma once

#include "GameFramework/Character.h"
#include "PS3PlayerCharacter.generated.h"

class UUW_HPText;
class UDXHPTextWidgetComponent;
class UDXStatusComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class PUZZLESTAY3_API APS3PlayerCharacter : public ACharacter
{
	GENERATED_BODY()


public:
	APS3PlayerCharacter();

	UFUNCTION(BlueprintPure, Category = "PS3|Character")
	bool CanUseFieldControls() const;

	void Move(const FVector2D& InMovementVector);
	void Look(const FVector2D& InLookVector);
	void StartJump();
	void StopJump();
	void TryInteract();
	void TryDropHeldObject();
	

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PS3|Character|Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PS3|Character|Components")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS3|Character|Interaction")
	float InteractionDistance = 300.0f;

private:
	UFUNCTION(Server, Reliable)
	void Server_TryInteract();

	UFUNCTION(Server, Reliable)
	void Server_TryDropHeldObject();
};
