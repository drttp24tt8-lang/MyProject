#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WistoriaTypes.h"
#include "WistoriaRPGDataAssets.generated.h"

UCLASS(BlueprintType)
class MYPROJECT_API UWistoriaItemDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FWistoriaInventoryItem Item;

	UFUNCTION(BlueprintPure, Category = "Wistoria|Data Assets")
	FWistoriaInventoryItem MakeInventoryItem(int32 QuantityOverride = 0) const;
};

UCLASS(BlueprintType)
class MYPROJECT_API UWistoriaQuestDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FWistoriaQuestRecord Quest;

	UFUNCTION(BlueprintPure, Category = "Wistoria|Data Assets")
	FWistoriaQuestRecord MakeQuestRecord(bool bStartActive = true) const;
};

UCLASS(BlueprintType)
class MYPROJECT_API UWistoriaStarterLoadoutDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	TArray<TObjectPtr<UWistoriaItemDataAsset>> StartingItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	TArray<TObjectPtr<UWistoriaQuestDataAsset>> StartingQuests;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout", meta = (ClampMin = "0"))
	int32 StartingGold = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	TArray<FName> UnlockedWeaponIds;

	UFUNCTION(BlueprintPure, Category = "Wistoria|Data Assets")
	TArray<FWistoriaInventoryItem> MakeStartingInventory() const;

	UFUNCTION(BlueprintPure, Category = "Wistoria|Data Assets")
	TArray<FWistoriaQuestRecord> MakeStartingQuests() const;
};
