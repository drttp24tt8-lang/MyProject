#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "WistoriaTypes.h"
#include "WistoriaSaveGame.generated.h"

UCLASS()
class MYPROJECT_API UWistoriaSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "SaveData")
	bool bSaveStarted = false;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData|Progression")
	int32 CharacterLevel = 1;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData|Progression")
	int32 CurrentXP = 0;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData|Progression")
	int32 Coins = 0;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData|Progression")
	int32 NewGamePlusCount = 0;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData|Progression")
	int32 AcademyRankScore = 0;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData|Progression")
	int32 TotalRaidClearsCount = 0;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData|Progression")
	TArray<FWistoriaAcademyRank> AcademyRanks;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData|Progression")
	FName CurrentAcademyRankId = NAME_None;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData|Progression")
	TArray<FName> UnlockedCosmeticCapeIds;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData|Inventory")
	TArray<FName> WeaponLog;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData|Inventory")
	TArray<FWistoriaInventoryItem> InventoryItems;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData|Inventory")
	TMap<EWistoriaEquipmentSlot, FWistoriaInventoryItem> EquippedItems;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData|Inventory")
	int32 Gold = 0;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData|Customization")
	FLinearColor HairColor = FLinearColor(0.08f, 0.06f, 0.05f, 1.0f);

	UPROPERTY(BlueprintReadWrite, Category = "SaveData|Customization")
	FLinearColor RobeColor = FLinearColor(0.05f, 0.12f, 0.55f, 1.0f);

	UPROPERTY(BlueprintReadWrite, Category = "SaveData|Skills")
	int32 AvailableSkillPoints = 0;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData|Skills")
	TMap<FName, FWistoriaSkillNode> SkillTree;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData|Companion")
	TMap<FName, FWistoriaCompanionBond> CompanionBonds;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData|FastTravel")
	TArray<FWistoriaTeleportPoint> UnlockedTeleportPoints;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData|Checkpoint")
	FName ActiveCheckpointId = NAME_None;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData|Checkpoint")
	FVector CheckpointLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData|Checkpoint")
	FRotator CheckpointRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData|Checkpoint")
	bool bHasCheckpoint = false;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData|Quest")
	TArray<FWistoriaQuestRecord> QuestRecords;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData|Story")
	TArray<FName> CompletedStoryFlags;
};
