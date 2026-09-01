#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dumbbell.generated.h"

UCLASS()
class PUZZLESTAY3_API ADumbbell : public AActor
{
	GENERATED_BODY()
	
public:	
	ADumbbell();
	
	// 캐릭터가 덤벨을 집을 때 호출 (서버 전용)
	void Grab(USceneComponent* TargetParent);

	// 캐릭터가 덤벨을 놓을 때 호출 (서버 전용)
	void Drop();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DumbbellMesh;

};
