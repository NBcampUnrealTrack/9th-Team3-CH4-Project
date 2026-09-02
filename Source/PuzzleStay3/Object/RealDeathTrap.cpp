#include "Object/RealDeathTrap.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"

ARealDeathTrap::ARealDeathTrap()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
	RootComponent = CollisionComp;
	
	CollisionComp->SetCollisionProfileName(TEXT("Trigger"));
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
}

void ARealDeathTrap::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ARealDeathTrap::OnOverlapBegin);
}

void ARealDeathTrap::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
								   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
								   bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this))
	{
		APawn* PlayerPawn = Cast<APawn>(OtherActor);
		if (PlayerPawn && PlayerPawn->IsPlayerControlled())
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, 
					FString::Printf(TEXT("플레이어 오버랩 감지: %s"), *OtherActor->GetName()));
			}
		}
	}
}

