#include "RaidSpawnManager.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "WistoriaEnemy.h"
#include "WistoriaRaidGameMode.h"

ARaidSpawnManager::ARaidSpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;

	ActivationVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("ActivationVolume"));
	RootComponent = ActivationVolume;
	ActivationVolume->SetBoxExtent(FVector(1200.0f, 1200.0f, 400.0f));
	ActivationVolume->SetCollisionProfileName(TEXT("Trigger"));

	LocalSpawnOffsets.Add(FVector(600.0f, 0.0f, 90.0f));
	LocalSpawnOffsets.Add(FVector(-600.0f, 0.0f, 90.0f));
	LocalSpawnOffsets.Add(FVector(0.0f, 600.0f, 90.0f));
	LocalSpawnOffsets.Add(FVector(0.0f, -600.0f, 90.0f));
	LocalSpawnOffsets.Add(FVector(425.0f, 425.0f, 90.0f));
	LocalSpawnOffsets.Add(FVector(-425.0f, -425.0f, 90.0f));
}

void ARaidSpawnManager::BeginPlay()
{
	Super::BeginPlay();
	ActivationVolume->OnComponentBeginOverlap.AddDynamic(this, &ARaidSpawnManager::OnActivationOverlap);
}

bool ARaidSpawnManager::StartRaid()
{
	if (bRaidStarted)
	{
		return false;
	}

	AWistoriaRaidGameMode* RaidGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AWistoriaRaidGameMode>() : nullptr;
	if (!RaidGameMode)
	{
		return false;
	}

	bRaidStarted = RaidGameMode->StartRaid(this);
	return bRaidStarted;
}

int32 ARaidSpawnManager::SpawnRaidWave(int32 EnemyCount, float HealthMultiplier, float DamageMultiplier, AWistoriaRaidGameMode* RaidGameMode, TSubclassOf<AWistoriaEnemy> OverrideFirstEnemyClass)
{
	if (!GetWorld() || !RaidGameMode || EnemyCount <= 0 || (EnemyClasses.Num() == 0 && !OverrideFirstEnemyClass))
	{
		return 0;
	}

	int32 SpawnedCount = 0;
	for (int32 Index = 0; Index < EnemyCount; ++Index)
	{
		const TSubclassOf<AWistoriaEnemy> EnemyClass = (Index == 0 && OverrideFirstEnemyClass)
			? OverrideFirstEnemyClass
			: (EnemyClasses.Num() > 0 ? EnemyClasses[FMath::RandRange(0, EnemyClasses.Num() - 1)] : nullptr);
		if (!EnemyClass)
		{
			continue;
		}

		AWistoriaEnemy* Enemy = GetWorld()->SpawnActor<AWistoriaEnemy>(
			EnemyClass,
			GetSpawnLocation(Index),
			GetActorRotation());

		if (!Enemy)
		{
			continue;
		}

		Enemy->ApplyScaling(HealthMultiplier, DamageMultiplier);
		RaidGameMode->RegisterSpawnedEnemy(Enemy);
		SpawnedCount++;
	}

	return SpawnedCount;
}

void ARaidSpawnManager::OnActivationOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bStartOnPlayerOverlap && OtherActor && OtherActor->IsA<APawn>())
	{
		StartRaid();
	}
}

FVector ARaidSpawnManager::GetSpawnLocation(int32 SpawnIndex) const
{
	if (WorldSpawnLocations.Num() > 0)
	{
		return WorldSpawnLocations[SpawnIndex % WorldSpawnLocations.Num()];
	}

	if (LocalSpawnOffsets.Num() == 0)
	{
		return GetActorLocation();
	}

	return GetActorTransform().TransformPosition(LocalSpawnOffsets[SpawnIndex % LocalSpawnOffsets.Num()]);
}
