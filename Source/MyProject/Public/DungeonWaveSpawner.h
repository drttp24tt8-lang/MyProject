#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonWaveSpawner.generated.h"

class UBoxComponent;
class UPrimitiveComponent;
class AWistoriaEnemy;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWaveStartedSignature, int32, WaveNumber, int32, EnemyCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaveCompletedSignature, int32, WaveNumber);

UCLASS()
class MYPROJECT_API ADungeonWaveSpawner : public AActor
{
	GENERATED_BODY()

public:
	ADungeonWaveSpawner();

	UFUNCTION(BlueprintCallable, Category = "Dungeon")
	void StartNextWave();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> TriggerVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon")
	TArray<TSubclassOf<AWistoriaEnemy>> EnemyClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon")
	TArray<FVector> LocalSpawnOffsets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon", meta = (ClampMin = "1"))
	int32 BaseEnemiesPerWave = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon", meta = (ClampMin = "0.1"))
	float CompanionHealthScale = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon", meta = (ClampMin = "0.0"))
	float CompanionDamageScale = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon", meta = (ClampMin = "0"))
	int32 ActiveCompanionCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeon")
	int32 CurrentWave = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeon")
	int32 RemainingEnemies = 0;

	UPROPERTY(BlueprintAssignable, Category = "Dungeon")
	FWaveStartedSignature OnWaveStarted;

	UPROPERTY(BlueprintAssignable, Category = "Dungeon")
	FWaveCompletedSignature OnWaveCompleted;

	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleEnemyDefeated(AWistoriaEnemy* Enemy);

private:
	FVector GetSpawnLocation(int32 SpawnIndex) const;
};
