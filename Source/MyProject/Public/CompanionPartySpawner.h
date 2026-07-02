#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CompanionPartySpawner.generated.h"

class AWistoriaCharacter;
class AWistoriaCompanionCharacter;
class UBoxComponent;
class UPrimitiveComponent;

UCLASS()
class MYPROJECT_API ACompanionPartySpawner : public AActor
{
	GENERATED_BODY()

public:
	ACompanionPartySpawner();

	UFUNCTION(BlueprintCallable, Category = "Companion")
	int32 SpawnCompanionParty(AWistoriaCharacter* PlayerCharacter);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> ActivationVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion")
	TArray<TSubclassOf<AWistoriaCompanionCharacter>> CompanionClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion")
	TArray<FVector> LocalSpawnOffsets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion", meta = (ClampMin = "1", ClampMax = "3"))
	int32 MaxCompanionsToSpawn = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion")
	bool bSpawnOnPlayerOverlap = true;

	UPROPERTY(BlueprintReadOnly, Category = "Companion")
	bool bHasSpawnedParty = false;

	UFUNCTION()
	void OnActivationOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	FVector GetSpawnLocation(int32 SpawnIndex) const;
};
