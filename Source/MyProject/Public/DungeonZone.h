#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonZone.generated.h"

class AWistoriaCharacter;
class UBoxComponent;
class UPrimitiveComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDungeonZoneEnteredSignature, int32, DungeonFloorLevel, AWistoriaCharacter*, PlayerCharacter);

UCLASS()
class MYPROJECT_API ADungeonZone : public AActor
{
	GENERATED_BODY()

public:
	ADungeonZone();

	UFUNCTION(BlueprintCallable, Category = "Dungeon")
	void SpawnMonsterWave(AActor* WaveInstigator = nullptr);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> ZoneTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon")
	int32 DungeonFloorLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon")
	TArray<TSubclassOf<AActor>> MonsterPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon", meta = (ClampMin = "1"))
	int32 MonstersToSpawn = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon")
	TArray<FVector> LocalSpawnOffsets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon")
	bool bTriggerOnce = true;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeon")
	bool bHasTriggered = false;

	UPROPERTY(BlueprintAssignable, Category = "Dungeon")
	FDungeonZoneEnteredSignature OnDungeonZoneEntered;

	UFUNCTION()
	void OnZoneEntered(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

private:
	FVector GetSpawnLocation(int32 SpawnIndex, const FTransform& SpawnOrigin) const;
};
