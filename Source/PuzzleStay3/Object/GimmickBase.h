#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GimmickBase.generated.h"

class USceneComponent;
class UStaticMeshComponent;

UCLASS(Abstract, Blueprintable)
class PUZZLESTAY3_API AGimmickBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AGimmickBase();
	
	UFUNCTION(BlueprintCallable, Category = "Gimmick")
	bool bCanInteract(const APawn* InteractingPawn) const;
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Gimmick")
	virtual void ResetGimmick();
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Gimmick")
	void HandleInteraction(APawn* InteractingPawn);
	
	UFUNCTION(BlueprintPure, Category = "Gimmick")
	bool IsGimmickActive() const { return bIsActive; }
	
	UFUNCTION(BlueprintPure, Category = "Gimmick")
	bool IsGimmickCompleted() const { return bIsCompleted; }

	UFUNCTION(BlueprintCallable, BlueprintCosmetic,
	Category = "Gimmick|Visual")
	void SetGimmickMeshVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic,
		Category = "Gimmick|Visual")
	void HideGimmickMesh();
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick")
	TObjectPtr<USceneComponent> Root;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick")
	TObjectPtr<UStaticMeshComponent> GimmickMesh;
	
	UPROPERTY(ReplicatedUsing = OnRep_IsActive, BlueprintReadOnly, Category = "Gimmick")
	bool bIsActive = true;

	UPROPERTY(ReplicatedUsing = OnRep_IsCompleted, BlueprintReadOnly, Category = "Gimmick")
	bool bIsCompleted = false;

	UFUNCTION()
	void OnRep_IsActive();

	UFUNCTION()
	void OnRep_IsCompleted();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Gimmick")
	void BP_OnInteraction(APawn* InteractingPawn);

	UFUNCTION(BlueprintImplementableEvent, Category = "Gimmick")
	void BP_OnActiveStateChanged(bool bNewActive);

	UFUNCTION(BlueprintImplementableEvent, Category = "Gimmick")
	void BP_OnCompleted();

public:	
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
