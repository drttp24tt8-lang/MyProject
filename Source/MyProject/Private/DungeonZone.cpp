#include "DungeonZone.h"

#include "Components/BoxComponent.h"
#include "WistoriaCharacter.h"

ADungeonZone::ADungeonZone()
{
	PrimaryActorTick.bCanEverTick = false;

	ZoneTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneTrigger"));
	RootComponent = ZoneTrigger;
	ZoneTrigger->SetBoxExtent(FVector(1000.0f, 1000.0f, 500.0f));
	ZoneTrigger->SetCollisionProfileName(TEXT("Trigger"));

	LocalSpawnOffsets.Add(FVector(450.0f, 0.0f, 80.0f));
	LocalSpawnOffsets.Add(FVector(-450.0f, 0.0f, 80.0f));
	LocalSpawnOffsets.Add(FVector(0.0f, 450.0f, 80.0f));
	LocalSpawnOffsets.Add(FVector(0.0f, -450.0f, 80.0f));
}

void ADungeonZone::BeginPlay()
{
	Super::BeginPlay();
	ZoneTrigger->OnComponentBeginOverlap.AddDynamic(this, &ADungeonZone::OnZoneEntered);
}

void ADungeonZone::OnZoneEntered(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	AWistoriaCharacter* Player = Cast<AWistoriaCharacter>(OtherActor);
	if (!Player || (bTriggerOnce && bHasTriggered))
	{
		return;
	}

	bHasTriggered = true;
	OnDungeonZoneEntered.Broadcast(DungeonFloorLevel, Player);
	SpawnMonsterWave(Player);

	if (bTriggerOnce)
	{
		ZoneTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ADungeonZone::SpawnMonsterWave(AActor* WaveInstigator)
{
	if (MonsterPool.Num() == 0 || !GetWorld())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Spawning magical beasts for dungeon floor %d."), DungeonFloorLevel);

	const FTransform SpawnOrigin = WaveInstigator ? WaveInstigator->GetActorTransform() : GetActorTransform();
	const int32 SpawnCount = FMath::Max(1, MonstersToSpawn);
	for (int32 Index = 0; Index < SpawnCount; ++Index)
	{
		const TSubclassOf<AActor> MonsterClass = MonsterPool[FMath::RandRange(0, MonsterPool.Num() - 1)];
		if (!MonsterClass)
		{
			continue;
		}

		GetWorld()->SpawnActor<AActor>(
			MonsterClass,
			GetSpawnLocation(Index, SpawnOrigin),
			GetActorRotation());
	}
}

FVector ADungeonZone::GetSpawnLocation(int32 SpawnIndex, const FTransform& SpawnOrigin) const
{
	if (LocalSpawnOffsets.Num() == 0)
	{
		return SpawnOrigin.GetLocation();
	}

	return SpawnOrigin.TransformPosition(LocalSpawnOffsets[SpawnIndex % LocalSpawnOffsets.Num()]);
}
