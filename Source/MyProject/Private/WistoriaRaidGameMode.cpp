#include "WistoriaRaidGameMode.h"

#include "RaidSpawnManager.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "WistoriaCharacter.h"
#include "WistoriaEnemy.h"
#include "WistoriaGameInstance.h"
#include "WistoriaInventoryComponent.h"
#include "WistoriaSaveGame.h"

AWistoriaRaidGameMode::AWistoriaRaidGameMode()
{
	RaidSnapshot.MaxWaves = MaxWaves;
}

void AWistoriaRaidGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoStartRaidOnBeginPlay)
	{
		TArray<AActor*> FoundManagers;
		UGameplayStatics::GetAllActorsOfClass(this, ARaidSpawnManager::StaticClass(), FoundManagers);
		if (FoundManagers.Num() > 0)
		{
			StartRaid(Cast<ARaidSpawnManager>(FoundManagers[0]));
		}
	}
}

bool AWistoriaRaidGameMode::StartRaid(ARaidSpawnManager* SpawnManager)
{
	if (!SpawnManager || RaidSnapshot.bRaidActive)
	{
		return false;
	}

	ActiveSpawnManager = SpawnManager;
	RaidSnapshot = FWistoriaRaidSnapshot();
	RaidSnapshot.bRaidActive = true;
	RaidSnapshot.MaxWaves = MaxWaves;
	BroadcastRaidState();
	StartNextWave();
	return true;
}

void AWistoriaRaidGameMode::StartNextWave()
{
	if (!ActiveSpawnManager || !RaidSnapshot.bRaidActive)
	{
		return;
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(NextWaveTimerHandle);
	}

	RaidSnapshot.CurrentWave++;
	RaidSnapshot.MaxWaves = MaxWaves;

	if (RaidSnapshot.CurrentWave > MaxWaves)
	{
		EndRaid(true);
		return;
	}

	const bool bFinalWave = RaidSnapshot.CurrentWave == MaxWaves;
	const int32 CompanionCount = RefreshActiveCompanionCountFromPlayer();
	const int32 NewGamePlusCount = GetActiveNewGamePlusCount();
	const int32 EnemyCount = bFinalWave
		? 1
		: FMath::Max(1, BaseEnemiesPerWave + (RaidSnapshot.CurrentWave - 1) * 2 + CompanionCount + NewGamePlusCount * ExtraEnemiesPerNewGamePlusLoop);
	const float HealthMultiplier = GetEnemyHealthMultiplier()
		+ (RaidSnapshot.CurrentWave - 1) * HealthScalePerWave
		+ static_cast<float>(NewGamePlusCount) * HealthScalePerNewGamePlusLoop;
	const float DamageMultiplier = GetEnemyDamageMultiplier()
		+ (RaidSnapshot.CurrentWave - 1) * DamageScalePerWave
		+ static_cast<float>(NewGamePlusCount) * DamageScalePerNewGamePlusLoop;

	OnRaidWaveStarted.Broadcast(RaidSnapshot.CurrentWave, EnemyCount);
	BroadcastRaidState();

	const int32 SpawnedCount = ActiveSpawnManager->SpawnRaidWave(
		EnemyCount,
		HealthMultiplier,
		DamageMultiplier,
		this,
		bFinalWave ? FinalBossClass : nullptr);
	if (SpawnedCount <= 0)
	{
		FailRaid();
	}
}

void AWistoriaRaidGameMode::RegisterSpawnedEnemy(AWistoriaEnemy* Enemy)
{
	if (!Enemy)
	{
		return;
	}

	Enemy->OnEnemyDefeated.AddDynamic(this, &AWistoriaRaidGameMode::HandleRaidEnemyDefeated);
	RaidSnapshot.AliveEnemies++;
	BroadcastRaidState();
}

void AWistoriaRaidGameMode::FailRaid()
{
	EndRaid(false);
}

void AWistoriaRaidGameMode::EndRaid(bool bPlayerWon)
{
	if (!RaidSnapshot.bRaidActive && !RaidSnapshot.bRaidComplete && !RaidSnapshot.bRaidFailed)
	{
		return;
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(NextWaveTimerHandle);
	}

	RaidSnapshot.bRaidActive = false;
	RaidSnapshot.bRaidComplete = bPlayerWon;
	RaidSnapshot.bRaidFailed = !bPlayerWon;
	if (bPlayerWon)
	{
		GrantRaidRewards();
	}
	BroadcastRaidState();
}

FWistoriaRaidSnapshot AWistoriaRaidGameMode::GetRaidSnapshot() const
{
	return RaidSnapshot;
}

void AWistoriaRaidGameMode::HandleRaidEnemyDefeated(AWistoriaEnemy* Enemy)
{
	RaidSnapshot.AliveEnemies = FMath::Max(0, RaidSnapshot.AliveEnemies - 1);
	RaidSnapshot.TotalEnemiesDefeated++;
	RaidSnapshot.RaidScore += ScorePerEnemy;
	BroadcastRaidState();

	if (RaidSnapshot.AliveEnemies == 0)
	{
		if (RaidSnapshot.CurrentWave >= MaxWaves)
		{
			CompleteRaid();
		}
		else
		{
			if (GetWorld())
			{
				GetWorld()->GetTimerManager().SetTimer(
					NextWaveTimerHandle,
					this,
					&AWistoriaRaidGameMode::StartNextWave,
					SecondsBetweenWaves,
					false);
			}
		}
	}
}

void AWistoriaRaidGameMode::CompleteRaid()
{
	EndRaid(true);
}

void AWistoriaRaidGameMode::BroadcastRaidState()
{
	OnRaidStateChanged.Broadcast(RaidSnapshot);
}

void AWistoriaRaidGameMode::GrantRaidRewards()
{
	AWistoriaCharacter* PlayerCharacter = Cast<AWistoriaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!PlayerCharacter)
	{
		return;
	}

	for (const FWistoriaInventoryItem& RewardItem : RaidClearRewards)
	{
		PlayerCharacter->AddInventoryItem(RewardItem);
	}

	if (RaidClearGoldReward > 0)
	{
		if (UWistoriaInventoryComponent* Inventory = PlayerCharacter->FindComponentByClass<UWistoriaInventoryComponent>())
		{
			Inventory->AddGold(RaidClearGoldReward);
		}
	}

	if (RaidClearXPReward > 0)
	{
		PlayerCharacter->AddXP(RaidClearXPReward);
	}

	PlayerCharacter->AwardAffinityToParty(50);
	PlayerCharacter->AddAcademyRankScore(RaidSnapshot.RaidScore);
	PlayerCharacter->AddRaidClearProgress(1);
}

int32 AWistoriaRaidGameMode::GetActiveNewGamePlusCount() const
{
	const UWistoriaGameInstance* WistoriaGameInstance = GetGameInstance<UWistoriaGameInstance>();
	if (!WistoriaGameInstance)
	{
		return 0;
	}

	const UWistoriaSaveGame* SaveGame = Cast<UWistoriaSaveGame>(UGameplayStatics::LoadGameFromSlot(WistoriaGameInstance->GetActiveSaveSlot(), 0));
	return SaveGame ? FMath::Max(0, SaveGame->NewGamePlusCount) : 0;
}
