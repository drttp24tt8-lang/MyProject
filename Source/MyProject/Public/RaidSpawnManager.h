#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RaidSpawnManager.generated.h"

class AWistoriaEnemy;
class AWistoriaRaidGameMode;
class UBoxComponent;

UCLASS()
class MYPROJECT_API ARaidSpawnManager : public AActor
{
	GENERATED_BODY()

public:
	ARaidSpawnManager();

	UFUNCTION(BlueprintCallable, Category = "Raid")
	bool StartRaid();

	UFUNCTION(BlueprintCallable, Category = "Raid")
	int32 SpawnRaidWave(int32 EnemyCount, float HealthMultiplier, float DamageMultiplier, AWistoriaRaidGameMode* RaidGameMode, TSubclassOf<AWistoriaEnemy> OverrideFirstEnemyClass = nullptr);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> ActivationVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raid")
	TArray<TSubclassOf<AWistoriaEnemy>> EnemyClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raid")
	TArray<FVector> LocalSpawnOffsets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raid")
	TArray<FVector> WorldSpawnLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raid")
	bool bStartOnPlayerOverlap = true;

	UPROPERTY(BlueprintReadOnly, Category = "Raid")
	bool bRaidStarted = false;

	UFUNCTION()
	void OnActivationOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	FVector GetSpawnLocation(int32 SpawnIndex) const;
};
