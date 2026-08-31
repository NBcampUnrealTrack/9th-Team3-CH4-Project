#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OverlapSwitchComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOverlapStateChanged, bool, bNewIsOverlapped);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PUZZLESTAY3_API UOverlapSwitchComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UOverlapSwitchComponent();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	//서버/클라이언트 동기화 변수
	UPROPERTY(ReplicatedUsing=OnRep_IsOverlapped, VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick|State")
	bool bIsOverlapped;
	
	//외부(GimmickBase 등)에서 바인딩할 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Gimmick|Event")
	FOnOverlapStateChanged OnOverlapStateChanged;
	
	//Trigger/Collision의 Begin/End Overlap 이벤트와 바인딩할 함수
	UFUNCTION(BlueprintCallable, Category = "Gimmick|Logic")
	void HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	
	UFUNCTION(BlueprintCallable, Category = "Gimmick|Logic")
	void HandleEndOverlap(AActor* OverlappedActor, AActor* OtherActor);
	
protected:
	UFUNCTION()
	void OnRep_IsOverlapped();

private:
	// 감지 대상 액터인지 검증하는 헬퍼 함수
	bool IsValidOverlapActor(AActor* TargetActor) const;
	
	// 현재 오버랩되어 있는 액터 개수 카운트 (서버 전용)
	int32 OverlappedActorCount = 0;
};
