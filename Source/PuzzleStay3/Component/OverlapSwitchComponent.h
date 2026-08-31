#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OverlapSwitchComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnOverlapStateChanged, bool);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PUZZLESTAY3_API UOverlapSwitchComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UOverlapSwitchComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//서버/클라이언트 동기화 변수
	UPROPERTY(ReplicatedUsing=OnRep_IsOverlapped, VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick|State")
	bool bIsOverlapped = false;
	
public:		
	//외부(GimmickBase 등)에서 바인딩할 델리게이트
	FOnOverlapStateChanged OnOverlapStateChanged;
	
	// GameMode의 AllOverlapSwitchActivated() 등에서 사용할 Getter
	UFUNCTION(BlueprintCallable, Category = "Gimmick")
	bool IsOverlapped() const { return bIsOverlapped; }
	
protected:
	// C++ 전용 Overlap 콜백 함수 (언리얼 델리게이트 시그니처에 맞춤)
	UFUNCTION()
	void OnOwnerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOwnerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	void OnRep_IsOverlapped();

private:
	// 감지 대상 액터인지 검증하는 헬퍼 함수
	bool IsValidOverlapActor(AActor* TargetActor) const;
	
	// 현재 오버랩되어 있는 액터 개수 카운트 (서버 전용)
	int32 OverlappedActorCount = 0;
};
