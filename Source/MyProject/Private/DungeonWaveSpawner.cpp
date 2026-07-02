#include "DungeonWaveSpawner.h"

#include "Components/BoxComponent.h"
#include "WistoriaCharacter.h"
#include "WistoriaEnemy.h"

ADungeonWaveSpawner::ADungeonWaveSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	SetRootComponent(TriggerVolume);
	TriggerVolume->SetBoxExtent(FVector(1000.0f, 1000.0f, 300.0f));
	TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));

	LocalSpawnOffsets.Add(FVector(400.0f, 0.0f, 80.0f));
	LocalSpawnOffsets.Add(FVector(-400.0f, 0.0f, 80.0f));
	LocalSpawnOffsets.Add(FVector(0.0f, 400.0f, 80.0f));
	LocalSpawnOffsets.Add(FVector(0.0f, -400.0f, 80.0f));
}

void ADungeonWaveSpawner::BeginPlay()
{
	Super::BeginPlay();

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ADungeonWaveSpawner::OnTriggerBeginOverlap);
}

void ADungeonWaveSpawner::StartNextWave()
{
	if (EnemyClasses.Num() == 0 || !GetWorld())
	{
		return;
	}

	CurrentWave++;

	const float PartyCountScale = 1.0f + static_cast<float>(ActiveCompanionCount) * 0.35f;
	const int32 EnemyCount = FMath::Max(1, FMath::RoundToInt(BaseEnemiesPerWave * CurrentWave * PartyCountScale));
	const float HealthMultiplier = 1.0f + static_cast<float>(ActiveCompanionCount) * CompanionHealthScale;
	const float DamageMultiplier = 1.0f + static_cast<float>(ActiveCompanionCount) * CompanionDamageScale;

	RemainingEnemies = EnemyCount;
	OnWaveStarted.Broadcast(CurrentWave, EnemyCount);

	for (int32 Index = 0; Index < EnemyCount; ++Index)
	{
		const int32 ClassIndex = FMath::RandRange(0, EnemyClasses.Num() - 1);
		TSubclassOf<AWistoriaEnemy> EnemyClass = EnemyClasses[ClassIndex];
		if (!EnemyClass)
		{
			RemainingEnemies--;
			continue;
		}

		AWistoriaEnemy* Enemy = GetWorld()->SpawnActor<AWistoriaEnemy>(
			EnemyClass,
			GetSpawnLocation(Index),
			GetActorRotation());

		if (!Enemy)
		{
			RemainingEnemies--;
			continue;
		}

		Enemy->ApplyScaling(HealthMultiplier, DamageMultiplier);
		Enemy->OnEnemyDefeated.AddDynamic(this, &ADungeonWaveSpawner::HandleEnemyDefeated);
	}

	if (RemainingEnemies <= 0)
	{
		OnWaveCompleted.Broadcast(CurrentWave);
	}
}

void ADungeonWaveSpawner::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	AWistoriaCharacter* PlayerCharacter = Cast<AWistoriaCharacter>(OtherActor);
	if (!PlayerCharacter || CurrentWave > 0)
	{
		return;
	}

	ActiveCompanionCount = FMath::Clamp(PlayerCharacter->GetActiveCompanions().Num(), 0, 3);
	StartNextWave();
	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADungeonWaveSpawner::HandleEnemyDefeated(AWistoriaEnemy* Enemy)
{
	RemainingEnemies = FMath::Max(0, RemainingEnemies - 1);

	if (RemainingEnemies == 0)
	{
		OnWaveCompleted.Broadcast(CurrentWave);
	}
}

FVector ADungeonWaveSpawner::GetSpawnLocation(int32 SpawnIndex) const
{
	if (LocalSpawnOffsets.Num() == 0)
	{
		return GetActorLocation();
	}

	return GetActorTransform().TransformPosition(LocalSpawnOffsets[SpawnIndex % LocalSpawnOffsets.Num()]);
}
