#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WistoriaGameMode.generated.h"

class AWistoriaCharacter;

UCLASS()
class MYPROJECT_API AWistoriaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AWistoriaGameMode();

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Party")
	void SetActiveCompanionCount(int32 NewCompanionCount);

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Party")
	void AddActiveCompanion();

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Party")
	void RemoveActiveCompanion();

	UFUNCTION(BlueprintPure, Category = "Wistoria|Party")
	int32 GetActiveCompanionCount() const;

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Party")
	int32 RefreshActiveCompanionCountFromPlayer();

	UFUNCTION(BlueprintPure, Category = "Wistoria|Difficulty")
	float GetEnemyHealthMultiplier() const;

	UFUNCTION(BlueprintPure, Category = "Wistoria|Difficulty")
	float GetEnemyDamageMultiplier() const;

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Progression")
	void AwardXPToPlayer(int32 XPAmount);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Party", meta = (ClampMin = "0", ClampMax = "3"))
	int32 ActiveCompanionCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Party")
	bool bAutoSyncCompanionCountFromPlayer = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Difficulty", meta = (ClampMin = "0.0"))
	float HealthScalePerCompanion = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Difficulty", meta = (ClampMin = "0.0"))
	float DamageScalePerCompanion = 0.25f;
};
