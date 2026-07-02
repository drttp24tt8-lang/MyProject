#pragma once

#include "CoreMinimal.h"
#include "WistoriaGameMode.h"
#include "WistoriaTypes.h"
#include "WistoriaRaidGameMode.generated.h"

class AWistoriaEnemy;
class ARaidSpawnManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRaidStateChangedSignature, const FWistoriaRaidSnapshot&, RaidSnapshot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRaidWaveStartedSignature, int32, WaveNumber, int32, EnemyCount);

UCLASS()
class MYPROJECT_API AWistoriaRaidGameMode : public AWistoriaGameMode
{
	GENERATED_BODY()

public:
	AWistoriaRaidGameMode();

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Raid")
	bool StartRaid(ARaidSpawnManager* SpawnManager);

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Raid")
	void StartNextWave();

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Raid")
	void RegisterSpawnedEnemy(AWistoriaEnemy* Enemy);

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Raid")
	void FailRaid();

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Raid")
	void EndRaid(bool bPlayerWon);

	UFUNCTION(BlueprintPure, Category = "Wistoria|Raid")
	FWistoriaRaidSnapshot GetRaidSnapshot() const;

	UPROPERTY(BlueprintAssignable, Category = "Wistoria|Raid")
	FRaidStateChangedSignature OnRaidStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Wistoria|Raid")
	FRaidWaveStartedSignature OnRaidWaveStarted;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Raid", meta = (ClampMin = "1"))
	int32 MaxWaves = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Raid", meta = (ClampMin = "1"))
	int32 BaseEnemiesPerWave = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Raid", meta = (ClampMin = "0.0"))
	float HealthScalePerWave = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Raid", meta = (ClampMin = "0.0"))
	float DamageScalePerWave = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Raid|New Game Plus", meta = (ClampMin = "0.0"))
	float HealthScalePerNewGamePlusLoop = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Raid|New Game Plus", meta = (ClampMin = "0.0"))
	float DamageScalePerNewGamePlusLoop = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Raid|New Game Plus", meta = (ClampMin = "0"))
	int32 ExtraEnemiesPerNewGamePlusLoop = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Raid", meta = (ClampMin = "0"))
	int32 ScorePerEnemy = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Raid", meta = (ClampMin = "0.0"))
	float SecondsBetweenWaves = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Raid")
	bool bAutoStartRaidOnBeginPlay = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Raid")
	TSubclassOf<AWistoriaEnemy> FinalBossClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Raid|Rewards")
	TArray<FWistoriaInventoryItem> RaidClearRewards;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Raid|Rewards", meta = (ClampMin = "0"))
	int32 RaidClearGoldReward = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Raid|Rewards", meta = (ClampMin = "0"))
	int32 RaidClearXPReward = 1000;

	UPROPERTY(BlueprintReadOnly, Category = "Wistoria|Raid")
	FWistoriaRaidSnapshot RaidSnapshot;

	UPROPERTY(BlueprintReadOnly, Category = "Wistoria|Raid")
	TObjectPtr<ARaidSpawnManager> ActiveSpawnManager;

	UFUNCTION()
	void HandleRaidEnemyDefeated(AWistoriaEnemy* Enemy);

private:
	void CompleteRaid();
	void BroadcastRaidState();
	void GrantRaidRewards();
	int32 GetActiveNewGamePlusCount() const;
	FTimerHandle NextWaveTimerHandle;
};
