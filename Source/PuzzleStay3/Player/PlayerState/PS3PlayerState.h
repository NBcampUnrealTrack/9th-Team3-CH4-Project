#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/PS3PlayerRole.h"
#include "GameFramework/PlayerState.h"
#include "PS3PlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPS3LifeCountChangedSignature, int32, NewLifeCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPS3DeadStateChangedSignature, bool, bNewIsDead);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPS3PlayerRoleChangedSignature, EPS3PlayerRole, NewRole);

UCLASS()
class PUZZLESTAY3_API APS3PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	APS3PlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "PS3|Player State")
	int32 GetCurrentLifeCount() const { return CurrentLifeCount; }

	UFUNCTION(BlueprintPure, Category = "PS3|Player State")
	bool IsDead() const { return bIsDead; }

	UFUNCTION(BlueprintPure, Category = "PS3|Player State")
	EPS3PlayerRole GetPlayerRole() const { return PlayerRole; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "PS3|Player State")
	void SetCurrentLifeCount(int32 NewLifeCount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "PS3|Player State")
	void SetIsDead(bool bNewIsDead);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "PS3|Player State")
	void SetPlayerRole(EPS3PlayerRole NewRole);

	UPROPERTY(BlueprintAssignable, Category = "PS3|Player State|Events")
	FPS3LifeCountChangedSignature OnLifeCountChanged;

	UPROPERTY(BlueprintAssignable, Category = "PS3|Player State|Events")
	FPS3DeadStateChangedSignature OnDeadStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "PS3|Player State|Events")
	FPS3PlayerRoleChangedSignature OnPlayerRoleChanged;

protected:
	UFUNCTION()
	void OnRep_LifeChanged();

	UFUNCTION()
	void OnRep_DeadChanged();

	UFUNCTION()
	void OnRep_PlayerRole();

	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_LifeChanged, Category = "PS3|Player State", meta = (ClampMin = "0"))
	int32 CurrentLifeCount = 3;

	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_DeadChanged, Category = "PS3|Player State")
	bool bIsDead = false;

	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_PlayerRole, Category = "PS3|Player State")
	EPS3PlayerRole PlayerRole = EPS3PlayerRole::Unassigned;
};
