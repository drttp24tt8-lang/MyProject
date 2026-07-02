#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WistoriaTypes.h"
#include "WistoriaDataLogLibrary.generated.h"

class UDataTable;
class UWistoriaItemDataAsset;
class UWistoriaQuestDataAsset;
class UWistoriaStarterLoadoutDataAsset;

UCLASS()
class MYPROJECT_API UWistoriaDataLogLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Wistoria|Data Logs")
	static bool GetWeaponRow(UDataTable* WeaponTable, FName WeaponId, FWistoriaWeaponDataRow& OutWeaponRow);

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Data Logs")
	static bool GetSpellRow(UDataTable* SpellTable, FName SpellId, FWistoriaSpellDataRow& OutSpellRow);

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Data Logs")
	static bool GetCraftingRecipeRow(UDataTable* CraftingRecipeTable, FName RecipeId, FWistoriaCraftingRecipe& OutRecipeRow);

	UFUNCTION(BlueprintPure, Category = "Wistoria|Data Logs")
	static FWistoriaInventoryItem MakeInventoryItemFromWeaponRow(const FWistoriaWeaponDataRow& WeaponRow);

	UFUNCTION(BlueprintPure, Category = "Wistoria|Data Assets")
	static FWistoriaInventoryItem MakeInventoryItemFromAsset(const UWistoriaItemDataAsset* ItemAsset, int32 QuantityOverride = 0);

	UFUNCTION(BlueprintPure, Category = "Wistoria|Data Assets")
	static FWistoriaQuestRecord MakeQuestRecordFromAsset(const UWistoriaQuestDataAsset* QuestAsset, bool bStartActive = true);

	UFUNCTION(BlueprintPure, Category = "Wistoria|Data Assets")
	static TArray<FWistoriaInventoryItem> MakeInventoryFromLoadout(const UWistoriaStarterLoadoutDataAsset* LoadoutAsset);

	UFUNCTION(BlueprintPure, Category = "Wistoria|Data Assets")
	static TArray<FWistoriaQuestRecord> MakeQuestsFromLoadout(const UWistoriaStarterLoadoutDataAsset* LoadoutAsset);
};
